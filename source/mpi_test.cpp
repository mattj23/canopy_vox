#include <mpi.h>
#include <iostream>
#include <unordered_map>
#include <memory>

#include "utilities.h"

enum class ProgramState {reading, thinning, reading2, thinning2, finalize};

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
};

class Director: public Process
{
public:
    Director(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    :Process(id, size, configuration, d)
    {

    }

    void run() override
    {
        // Wait for all of the readers to say they're done

        // Tell the workers to start thinning

        // Wait for the workers to say they're done

        // Tell the readers to begin stage 2

        // Wait for the readers to say they're done

        // Tell the workers to start thinning

        // The Director's job is done, the Workers will finish from here
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
        // Start with reading and transmitting all of the files
        for (auto f : files)
            readFile(f);

        // Tell the Director we're done

        // Wait for the director to tell us to proceed

        // Read and transmit all of the scratch files

        // Now we're finished and the process can end
    }

private:
    std::vector<std::string> files;
    std::unordered_map<size_t, std::vector<Vector3d>> transmitBuffers;

    void readFile(std::string fileName)
    {

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

        // Write the intermediate files to the scratch directory

        // Tell the director that we're done

        // Wait for incoming data: if it's from the readers add it to our local
        // buffers and sort it into place, if it's from the Director start
        // doing the thinning

        // Write the intermediate files to the scratch directory

        // Perform the final voxelization


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
