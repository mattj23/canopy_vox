#include <mpi.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <memory>
#include <unordered_map>

#include "utilities.h"
#include "vector3d.h"
#include "pointcloud.h"
#include "voxelsorter.h"

enum class ProgramState {reading, thinning, reading2, thinning2, finalize};
enum class MessageInfo {readerDone, workerDone, startWorking};
enum class WorkerTypes {director, reader, worker};

/* The Directory class takes the MPI world size and the configuration settings
and from it computes the task assignments of all of the processes in the MPI
world.  This is computed by each process in a deterministic way in order to
eliminate an initial step of communication between processes.  Additionaly,
the Directory class contains helper methods for addressing and communication
between processes. */
class Directory
{
public:
    Directory(size_t worldSize, const ParallelConfiguration &config)
    {
        // The 0-th worker is always the director
        mapping[0] = WorkerTypes::director;

        // If there are i input files and n processes, there will be n/4 readers
        // where there is at least one reader and no more than the number of
        // input files.
        nReaders = (size_t)((float)worldSize / 4.0);
        if (nReaders < 1)
            nReaders  = 1;
        if (nReaders > config.inputFiles.size())
            nReaders = config.inputFiles.size();

        nWorkers = worldSize - 1 - nReaders;

        for (size_t i = 1; i < worldSize; i++)
        {
            if (i <= nReaders)
                mapping[i] = WorkerTypes::reader;
            else
                mapping[i] = WorkerTypes::worker;
        }
    }

    inline size_t numberOfReaders() { return nReaders; }
    inline size_t numberOfWorkers() { return nWorkers; }
    inline WorkerTypes getProcessType(size_t rank) { return mapping[rank]; }

    inline size_t director() {return 0;}
    inline size_t workerByNumber(size_t worker) { return nReaders + 1 + worker; }
    inline size_t readerByNumber(size_t reader) { return 1 + reader; }
    inline size_t readerFromRank(size_t rank) { return rank - 1; }
    inline size_t workerFromRank(size_t rank) { return rank - 1 - nReaders; }

    void sendToDirector(MessageInfo info)
    {
        int messageCode = static_cast<int>(info);
        MPI_Send(&messageCode, 1, MPI_INT, director(), 0, MPI_COMM_WORLD);
    }

    void tellProcessToStart(size_t processRank)
    {
        int messageCode = static_cast<int>(MessageInfo::startWorking);
        MPI_Send(&messageCode, 1, MPI_INT, processRank, 0, MPI_COMM_WORLD);
    }

protected:
    size_t nReaders;
    size_t nWorkers;

    std::unordered_map<size_t, WorkerTypes> mapping;
};

class Process
{
public:

    Process(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    {
        worldId = id;
        worldSize = size;
        programState = ProgramState::reading;
        directory = d;
        config = configuration;
    }

    virtual void run() = 0;

protected:
    size_t worldId;
    size_t worldSize;
    ParallelConfiguration config;
    ProgramState programState;
    std::shared_ptr<Directory> directory;

    void waitForStartInstruction()
    {
        int rawMessageCode;
        MPI_Status status;
        bool gotStartMessage = false;
        while (!gotStartMessage)
        {
            MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            MPI_Recv(&rawMessageCode, 1, MPI_INT, MPI_ANY_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

            if (static_cast<MessageInfo>(rawMessageCode) == MessageInfo::startWorking)
            {
                gotStartMessage = true;
            }
        }
    }
};

class Director: public Process
{
public:
    Director(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    :Process(id, size, configuration, d)
    {
        for (size_t i = 0; i < directory->numberOfReaders(); i++)
            readers.push_back(false);

        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            workers.push_back(false);
    }

    void run() override
    {
        // Wait for all of the readers to say they're done
        waitFor(readers, MessageInfo::readerDone);
        std::cout << "All readers done! " << std::endl;

        // Tell the workers to start thinning
        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            directory->tellProcessToStart(directory->workerByNumber(i));

        // Wait for the workers to say they're done
        waitFor(workers, MessageInfo::workerDone);
        std::cout << "All workers done!" << std::endl;

        // Tell the readers to begin stage 2
        for (size_t i = 0; i < directory->numberOfReaders(); i++)
            directory->tellProcessToStart(directory->readerByNumber(i));

        // Wait for the readers to say they're done
        waitFor(readers, MessageInfo::readerDone);
        std::cout << "All readers done! " << std::endl;

        // Tell the workers to start thinning
        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            directory->tellProcessToStart(directory->workerByNumber(i));

        // The Director's job is done, the Workers will finish from here
    }

private:
    std::vector<bool> readers;
    std::vector<bool> workers;

    bool areDone(const std::vector<bool> &vectorOfBools)
    {
        for (auto b : vectorOfBools)
            if (!b)
                return false;
        return true;
    }

    void waitFor(std::vector<bool> &vectorOfBools, MessageInfo code)
    {
        // Reset the readers
        for (size_t i = 0; i < vectorOfBools.size(); i++)
            vectorOfBools[i] = false;

        int rawMessageCode;
        MPI_Status status;
        while (!areDone(vectorOfBools))
        {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            if (status.MPI_TAG == 0)
            {
                MPI_Recv(&rawMessageCode, 1, MPI_INT, MPI_ANY_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
            }

            if (static_cast<MessageInfo>(rawMessageCode) == code)
            {
                int processNumber;
                if (code == MessageInfo::readerDone)
                    processNumber = directory->readerFromRank(status.MPI_SOURCE);
                if (code == MessageInfo::workerDone)
                    processNumber = directory->workerFromRank(status.MPI_SOURCE);

                vectorOfBools[processNumber] = true;
            }
        }
    }
};

class Reader: public Process
{
public:
    Reader(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    :Process(id, size, configuration, d)
    {
        // The readerNumber is the 0-index rank of the current reader
        size_t readerNumber = worldId - 1;

        // Figure out which files this reader is supposed to read
        for (size_t i = 0; i < config.inputFiles.size(); i++)
        {
            if (i % directory->numberOfReaders() == readerNumber)
                files.push_back(config.inputFiles[i]);
        }
    }

    void run() override
    {
        // Construct the first stage voxel sorter
        sorter.reset(new VoxelSorter(10, 10, 10, 5, 5, 5));

        // Start with reading and transmitting all of the files
        for (auto f : files)
            readFile(f);

        // Tell the Director we're done
        directory->sendToDirector(MessageInfo::readerDone);

        // Wait for the director to tell us to proceed
        // waitForStartInstruction();

        // Read and transmit all of the scratch files

        // Now we're finished and the process can end
    }

private:
    std::vector<std::string> files;
    std::unordered_map<size_t, std::vector<Vector3d>> transmitBuffers;
    std::unique_ptr<VoxelSorter> sorter;

    void readFile(std::string fileName)
    {
        std::unordered_map<VoxelAddress, int> allVoxels;

        std::ifstream workingFile(fileName);
        std::string workingLine;

        double sx, sy, sz;

        while (std::getline(workingFile, workingLine))
        {
            std::istringstream i(workingLine);
            std::vector<std::string> tokens{std::istream_iterator<std::string>(i), std::istream_iterator<std::string>()};

            if (tokens.size() < 3)
                continue;

            sx = std::stod(tokens[0]);
            sy = std::stod(tokens[1]);
            sz = std::stod(tokens[2]);
            Vector3d v(sx, sy, sz);
            auto point = sorter->identifyPoint(v);
            allVoxels[point.address] = 0;

        }

        std::cout<<"File voxels total: " << allVoxels.size() << std::endl;
    }
};

class Worker: public Process
{
public:
    Worker(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    :Process(id, size, configuration, d)
    {

    }

    void run() override
    {
        // Wait for incoming data: if it's from the readers add it to our local
        // buffers and sort it into place, if it's from the Director start
        // doing the thinning
        receiveData();

        // Do the thinning

        // Write the intermediate files to the scratch directory

        // Tell the director that we're done
        directory->sendToDirector(MessageInfo::workerDone);

        // Wait for incoming data: if it's from the readers add it to our local
        // buffers and sort it into place, if it's from the Director start
        // doing the thinning

        // Write the intermediate files to the scratch directory

        // Perform the final voxelization


    }

private:
    std::unordered_map<VoxelAddress, std::vector<Vector3d>> rawData;

    void receiveData()
    {
        rawData.clear();

        MPI_Status status;
        bool isReceiving = true;
        while (isReceiving)
        {
            MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            // Tag 0 means this is an administrative message
            if (status.MPI_TAG == 0)
            {
                int rawMessageCode;
                MPI_Recv(&rawMessageCode, 1, MPI_INT, MPI_ANY_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);
                if (static_cast<MessageInfo>(rawMessageCode) == MessageInfo::startWorking)
                    isReceiving = false;
            }

            // Tag 1 means this is raw data
            else if (status.MPI_TAG == 1)
            {
                int recvCount;
                MPI_Get_count(&status, MPI_DOUBLE, &recvCount);
            }
        }
    }

};


int main(int argc, char** argv)
{
    // Get MPI world size and rank
    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Load the configuration input file
    if (argc < 2)
    {
        return -1;
    }

    auto config = LoadParallelConfiguration(argv[1]);

    // Construct the process directory, which allows this process to know the
    // ranks and assignments of the various other processes.
    std::shared_ptr<Directory> processDirectory(new Directory(world_size, config));

    // Construct and run the process object
    std::unique_ptr<Process> processWorker;
    switch (processDirectory->getProcessType(world_rank))
    {
        case WorkerTypes::director:
            processWorker.reset(new Director(world_rank, world_size, config, processDirectory));
            break;
        case WorkerTypes::reader:
            processWorker.reset(new Reader(world_rank, world_size, config, processDirectory));
            break;
        case WorkerTypes::worker:
            processWorker.reset(new Worker(world_rank, world_size, config, processDirectory));
            break;
    }
    processWorker->run();

    MPI_Finalize();
}
