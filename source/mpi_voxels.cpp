#include <mpi.h>

#include <set>
#include <cmath>
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <memory>
#include <unordered_map>
#include <cstdio>

#include <thread>
#include <chrono>

#include "nanoflann.hpp"
#include "utilities.h"
#include "vector3d.h"
#include "pointcloud.h"
#include "voxelsorter.h"

using namespace nanoflann;

#define MAX_SEND_SIZE 100 // When the transmit buffers get to this size they send
#define START_DELAY 1   // Number of seconds to delay non-director start

enum class ProgramState {reading, thinning, reading2, thinning2, finalize};
enum class MessageInfo {readerDone, workerDone, startWorking};
enum class WorkerTypes {director, reader, worker};

/// The Directory class takes the MPI world size and the configuration settings
/// and from it computes the task assignments of all of the processes in the MPI
/// world.  This is computed by each process in a deterministic way in order to
/// eliminate an initial step of communication between processes.  Additionaly,
/// the Directory class contains helper methods for addressing and communication
/// between processes. Think of this as the phone book for inter-process
/// communication.
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

    /// Sends an administrative message to the Director process based on the
    /// MessageInfo enum given
    void sendToDirector(MessageInfo info)
    {
        int messageCode = static_cast<int>(info);
        MPI_Send(&messageCode, 1, MPI_INT, director(), 0, MPI_COMM_WORLD);
    }

    /// Intended for use by the Director process, sends a start signal to
    /// the specified process number
    void tellProcessToStart(size_t processRank)
    {
        int messageCode = static_cast<int>(MessageInfo::startWorking);
        MPI_Send(&messageCode, 1, MPI_INT, processRank, 0, MPI_COMM_WORLD);
    }

protected:
    size_t nReaders;
    size_t nWorkers;

    // This is an internal mapping of process number to type of role
    std::unordered_map<size_t, WorkerTypes> mapping;
};


/// An abstract class to serve as a base for the three different process roles,
/// the Director, the Worker, and the Reader.  Contains an abstract method "run"
// that gets called when the process is instantiated.
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

    virtual std::string name() = 0;

protected:
    size_t worldId;
    size_t worldSize;
    ParallelConfiguration config;
    ProgramState programState;
    std::shared_ptr<Directory> directory;
    std::unique_ptr<VoxelSorter> sorter;

    /// Initializes the process' internal VoxelSorter with the information from
    /// the configuration object and a flag that determines whether the bins
    /// are shifted by half of the bin spacing (used in the initial sorting step)
    void initializeSorter(bool isShifted)
    {
        int mult = 0;
        while (config.voxelDistance * ++mult < config.binningDistance);
        double dv = config.voxelDistance * mult;
        // std::cout << "  grouping distance = " << dv << std::endl;
        if (isShifted)
            sorter.reset(new VoxelSorter(dv, dv, dv, dv/2.0, dv/2.0, dv/2.0));
        else
            sorter.reset(new VoxelSorter(dv, dv, dv, 0, 0, 0));
    }

    /// Blocks the process thread until an MPI communication is recieved with a
    /// MessageInfo::startWorking signal.
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

/// The Director process controls the entire algorithm, and serves as a relay
/// point for synchronizing actions between the different processes. The
/// Director's primary task is to track when all Readers or Workers are done
/// with a particular stage of work and signal to all of the processes to
/// begin the next step.
class Director: public Process
{
public:
    Director(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    :Process(id, size, configuration, d)
    {
        std::cout << "Director (process rank " << worldId << ") checking in" << std::endl;
        std::cout << "  -> Total number of MPI processes: " << size << std::endl;
        std::cout << "  -> Total number of Workers:       " << directory->numberOfWorkers() << std::endl;
        std::cout << "  -> Total number of Readers:       " << directory->numberOfReaders() << std::endl;
        std::cout << "  -> Configuration details: " << std::endl;
        PrintConfigDetails(config, 14);

        for (size_t i = 0; i < directory->numberOfReaders(); i++)
            readers.push_back(false);

        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            workers.push_back(false);

    }

    std::string name() override { return "Director"; }

    void run() override
    {
        // Wait for all of the readers to say they're done
        waitFor(readers, MessageInfo::readerDone);
        std::cout << "Director has confirmed that all readers have finished distributing stage 1 data" << std::endl;

        // Tell the workers to start thinning
        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            directory->tellProcessToStart(directory->workerByNumber(i));

        // Wait for the workers to say they're done
        waitFor(workers, MessageInfo::workerDone);
        std::cout << "Director has confirmed that all workers have finished stage 1 thinning" << std::endl;

        // Tell the readers to begin stage 2
        for (size_t i = 0; i < directory->numberOfReaders(); i++)
            directory->tellProcessToStart(directory->readerByNumber(i));

        // Wait for the readers to say they're done
        waitFor(readers, MessageInfo::readerDone);
        std::cout << "Director has confirmed that all readers have finished distributing stage 2 data" << std::endl;

        // Tell the workers to start thinning
        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            directory->tellProcessToStart(directory->workerByNumber(i));

        waitFor(workers, MessageInfo::workerDone);
        std::cout << "Director has confirmed that all workers have finished stage 2 thinning and sorting" << std::endl;
        std::cout << "Director reports that the run is now complete" << std::endl;

        // Combine the files
        combineResults();
    }

private:
    std::vector<bool> readers;
    std::vector<bool> workers;

    void combineResults()
    {
        std::string outputFileName = "combined_results.sparsevox";
        std::string line;
        std::ofstream outputFile(outputFileName);
        if (!outputFile.is_open())
        {
            std::cout << "Error opening file " << outputFileName << " for output!" << std::endl;
            return;
        }

        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
        {
            std::string readFileName = config.scratchDirectory + "worker" + std::to_string(i) + "_final.sparsevox";
            std::cout << "Director is combining results from " << readFileName << std::endl;
            std::ifstream infile(readFileName);
            if (infile.is_open())
            {
                while (std::getline(infile, line))
                {
                    outputFile << line << std::endl;
                }
            }
            infile.close();
            std::remove(readFileName.c_str());
        }

        outputFile.close();
    }

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
        // Delay this thread in order to let the Director print to stdout uninterrupted
        std::this_thread::sleep_for(std::chrono::seconds(2));

        readerNumber = directory->readerFromRank(worldId);
        std::cout << "Reader " << readerNumber << " (process rank " << worldId << ") checking in" << std::endl;

        // Figure out which files this reader is supposed to read
        files = getMyFilesFromList(config.inputFiles);
    }

    std::string name() override { return "Reader " + std::to_string(directory->readerFromRank(worldId)); }

    void run() override
    {
        // Construct the first stage voxel sorter
        initializeSorter(true);

        // Start with reading and transmitting all of the files
        for (auto f : files)
            readFile(f);

        // Tell the Director we're done
        directory->sendToDirector(MessageInfo::readerDone);

        // Wait for the director to tell us to proceed
        waitForStartInstruction();

        // Reset the sorter to the unshifted position or the second stage
        initializeSorter(false);

        // There should be one scratch file per worker
        std::vector<std::string> scratchFiles;
        for (size_t i = 0; i < directory->numberOfWorkers(); i++)
            scratchFiles.push_back(config.scratchDirectory + "worker" + std::to_string(i) + ".binary");
        files = getMyFilesFromList(scratchFiles);

        // Read and transmit all of the scratch files
        for (auto f: files)
            readBinaryFile(f);

        // Tell the Director we're done
        directory->sendToDirector(MessageInfo::readerDone);

        // Now we're finished and the process can end
    }

private:
    std::vector<std::string> files;
    std::unordered_map<size_t, std::vector<Vector3d>> transmitBuffers;
    std::hash<VoxelAddress> hasher;
    size_t readerNumber;
    double sendBuffer[MAX_SEND_SIZE * 3];

    std::vector<std::string> getMyFilesFromList(std::vector<std::string> allFiles)
    {
        std::vector<std::string> v;
        for (size_t i = 0; i < allFiles.size(); i++)
        {
            if (i % directory->numberOfReaders() == readerNumber)
                v.push_back(allFiles[i]);
        }
        return v;
    }

    void sendVectorsToWorker(size_t workerNumber, const std::vector<Vector3d> &sendList)
    {
        // Load the buffer
        size_t i = 0;
        for (Vector3d v : sendList)
        {
            sendBuffer[i++] = v.z;
            sendBuffer[i++] = v.y;
            sendBuffer[i++] = v.x;
        }

        MPI_Send(sendBuffer, i, MPI_DOUBLE, directory->workerByNumber(workerNumber), 1, MPI_COMM_WORLD);
    }


    void readBinaryFile(std::string fileName)
    {
        transmitBuffers.clear();

        size_t totalCount = 0;
        std::cout << "Reader " << directory->readerFromRank(worldId) << " is processing " << fileName << std::endl;
        std::ifstream fileStream(fileName, std::ios::binary);

        double ix, iy, iz;
        while(fileStream.read(reinterpret_cast<char *>(&ix), sizeof(ix)))
        {
            fileStream.read(reinterpret_cast<char *>(&iy), sizeof(iy));
            fileStream.read(reinterpret_cast<char *>(&iz), sizeof(iz));
            Vector3d v(ix, iy, iz);
            totalCount++;
            VoxelAddress address = sorter->identify(ix, iy, iz);

            size_t worker = hasher(address) % directory->numberOfWorkers();

            // Retrieve the transmit buffer for this worker
            auto mapIterator = transmitBuffers.find(worker);
            if (mapIterator == transmitBuffers.end())
                transmitBuffers[worker] = std::vector<Vector3d>();
            transmitBuffers[worker].push_back(v);

            // std::cout << "Reader " << directory->readerFromRank(worldId) << " read point " << v << " for " << worker << std::endl;

            // Send the transmit buffer if it's ready
            if (transmitBuffers[worker].size() > MAX_SEND_SIZE - 1)
            {
                if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " transmitting points to worker " << worker << std::endl;
                sendVectorsToWorker(worker, transmitBuffers[worker]);
                transmitBuffers[worker].clear();
            }
        }

        // Clear the remaining transmit buffers
        for (auto pair : transmitBuffers)
        {
            if (pair.second.size() > 0)
            {
                sendVectorsToWorker(pair.first, pair.second);
                pair.second.clear();
            }
        }

        // Delete the binary file when we're done with it
        std::cout << name() << " is deleting " << fileName << std::endl;
        std::remove(fileName.c_str());
    }

    void readFile(std::string fileName)
    {
        transmitBuffers.clear();

        std::cout << "Reader " << readerNumber << " is processing " << fileName << std::endl;

        std::ifstream workingFile(fileName);
        std::string workingLine;

        if (!workingFile.good())
        {
            std::cout << "Reader " << readerNumber << " found that file " << fileName << " could not be read!" << std::endl;
        }

        double sx, sy, sz;

        size_t count = 0;

        while (std::getline(workingFile, workingLine))
        {
            std::istringstream i(workingLine);
            std::vector<std::string> tokens{std::istream_iterator<std::string>(i), std::istream_iterator<std::string>()};

            if (tokens.size() < 3)
                continue;

            count++;
            // Read the values in the text lines
            if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " read tokens " << tokens[0] << ", " << tokens[1] << ", " << tokens[2] << std::endl;
            sx = std::stod(tokens[0]);
            sy = std::stod(tokens[1]);
            sz = std::stod(tokens[2]);
            if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " converted floats " << sx << ", " << sy << ", " << sz << std::endl;

            // Create the vector object and the voxel address
            Vector3d v(sx, sy, sz);
            VoxelAddress address = sorter->identify(sx, sy, sz);

            if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " sorted point " << v << " into address " << address << std::endl;

            size_t h = hasher(address);
            if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " hashed address " << address << " to value " << h << std::endl;
            if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " knows there are " << directory->numberOfWorkers() << " workers" << std::endl;


            size_t worker = hasher(address) % directory->numberOfWorkers();
            if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " assigned point " << v << " to Worker " << worker << std::endl;

            // Retrieve the transmit buffer for this worker
            auto mapIterator = transmitBuffers.find(worker);
            if (mapIterator == transmitBuffers.end())
                transmitBuffers[worker] = std::vector<Vector3d>();
            transmitBuffers[worker].push_back(v);

            // std::cout << "Reader " << directory->readerFromRank(worldId) << " read point " << v << " for " << worker << std::endl;

            // Send the transmit buffer if it's ready
            if (transmitBuffers[worker].size() > MAX_SEND_SIZE - 1)
            {
                if (config.debug) std::cout << "(DEBUG) Reader " << readerNumber << " transmitting points to worker " << worker << std::endl;

                sendVectorsToWorker(worker, transmitBuffers[worker]);
                transmitBuffers[worker].clear();
            }
        }

        // Clear the remaining transmit buffers
        for (auto pair : transmitBuffers)
        {
            if (pair.second.size() > 0)
            {
                sendVectorsToWorker(pair.first, pair.second);
                pair.second.clear();
            }
        }
    }
};

class Worker: public Process
{
public:
    Worker(size_t id, size_t size, const ParallelConfiguration &configuration, std::shared_ptr<Directory> d)
    :Process(id, size, configuration, d)
    {
        // Delay this thread in order to let the Director print to stdout uninterrupted
        std::this_thread::sleep_for(std::chrono::seconds(2));

        workerNumber = directory->workerFromRank(worldId);
        std::cout << "Worker " << workerNumber << " (process rank " << worldId << ") checking in" << std::endl;
    }

    std::string name() override { return "Worker " + std::to_string(directory->workerFromRank(worldId)); }

    void run() override
    {
        // Construct the first stage voxel sorter
        initializeSorter(true);

        // Wait for incoming data: if it's from the readers add it to our local
        // buffers and sort it into place, if it's from the Director start
        // doing the thinning
        receiveData();

        // Do the thinning
        size_t original = totalPoints();
        for (auto pair : rawData)
        {
            thinRegion(rawData[pair.first]);
        }

        writeBinaryRegions(config.scratchDirectory + "worker" + std::to_string(workerNumber) + ".binary");
        std::cout << "Worker " << workerNumber << " has thined " << rawData.size() << " regions" << std::endl;

        // Write the intermediate files to the scratch directory
        // Tell the director that we're done
        directory->sendToDirector(MessageInfo::workerDone);

        // Reset the sorter to the second stage position
        initializeSorter(false);

        // Wait for incoming data: if it's from the readers add it to our local
        // buffers and sort it into place, if it's from the Director start
        // doing the thinning
        receiveData();

        // Do the thinning
        original = totalPoints();
        for (auto pair : rawData)
        {
            thinRegion(rawData[pair.first]);
        }

        std::cout << "Worker " << workerNumber << " has thinned " << rawData.size() << " regions" << std::endl;

        // Perform the final voxelization
        VoxelSorter finalSorter(config.voxelDistance, config.voxelDistance, config.voxelDistance, 0, 0, 0);

        size_t count = 0;
        std::unordered_map<VoxelAddress, int> voxels;
        for (auto pair : rawData)
        {
            for (auto p : pair.second.pts)
            {
                auto located = finalSorter.identifyPoint(p);
                incrementVoxelIntensity(voxels, located.address);
            }
        }

        std::string outputFile = config.scratchDirectory + "worker" + std::to_string(workerNumber) + "_final.sparsevox";
        std::ofstream outfile;
        outfile.open(outputFile.c_str(), std::ios::out);

        for (auto voxel : voxels)
        {
            count += voxel.second;
            outfile << voxel.first.i << "," << voxel.first.j << "," << voxel.first.k << "," << voxel.second << std::endl;
        }

        // Tell the director we're done
        directory->sendToDirector(MessageInfo::workerDone);
    }

private:
    std::unordered_map<VoxelAddress, PointCloud> rawData;
    double recvBuffer[MAX_SEND_SIZE * 3];

    size_t workerNumber;

    size_t totalPoints()
    {
        size_t count = 0;
        for (auto pair : rawData)
        {
            count += pair.second.size();
        }
        return count;
    }

    void receiveData()
    {
        size_t totalRecv = 0;

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
                if (config.debug) std::cout << "(DEBUG) Worker " << workerNumber << " preparing to recieve data" << std::endl;
                int recvCount;
                MPI_Get_count(&status, MPI_DOUBLE, &recvCount);
                MPI_Recv(recvBuffer, recvCount, MPI_DOUBLE, MPI_ANY_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &status);

                if (config.debug) std::cout << "(DEBUG) Worker " << workerNumber << " recieved " << recvCount << " doubles" << std::endl;

                // Unpack the buffer
                for (size_t i = 0; i < recvCount; )
                {
                    Vector3d v(recvBuffer[i++], recvBuffer[i++], recvBuffer[i++]);
                    // std::cout << "Worker " << directory->workerFromRank(worldId) << " received " << v << std::endl;

                    auto located = sorter->identifyPoint(v);
                    auto mapIterator = rawData.find(located.address);
                    if (mapIterator == rawData.end())
                        rawData[located.address] = PointCloud();
                    rawData[located.address].pts.push_back(v);

                    totalRecv++;
                }
            }
        }

    }

    void thinRegion(PointCloud &cloud)
    {
        typedef KDTreeSingleIndexAdaptor<L2_Simple_Adaptor<double, PointCloud> ,PointCloud,3> my_kd_tree_t;
        my_kd_tree_t index(3, cloud, KDTreeSingleIndexAdaptorParams(10));
        index.buildIndex();

        std::set<size_t> removeIndicies;
        const double searchRadius = config.thinningDistance * config.thinningDistance;
        for (size_t i = 0; i < cloud.pts.size(); i++)
        {
            if (removeIndicies.find(i) == removeIndicies.end())
            {
                double query_pt[3] = { cloud.pts[i].x, cloud.pts[i].y, cloud.pts[i].z};

                std::vector<std::pair<size_t,double>> indices_dists;
                RadiusResultSet<double,size_t> resultSet(searchRadius, indices_dists);
                index.findNeighbors(resultSet, query_pt, nanoflann::SearchParams());

                for (auto r : resultSet.m_indices_dists)
                {
                    if (i != r.first)
                        removeIndicies.insert(r.first);
                }
            }
        }
        cloud.RemoveAtIndicies(removeIndicies);
    }

    void writeBinaryRegions(std::string fileName)
    {
        std::ofstream fileStream(fileName.c_str(), std::ios::binary);
        for (auto pair : rawData)
        {
            for (auto p : pair.second.pts)
            {
                // .write(reinterpret_cast<char *>(&x), sizeof(x))
                fileStream.write(reinterpret_cast<char*>(&p.x), sizeof(p.x));
                fileStream.write(reinterpret_cast<char*>(&p.y), sizeof(p.y));
                fileStream.write(reinterpret_cast<char*>(&p.z), sizeof(p.z));
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
