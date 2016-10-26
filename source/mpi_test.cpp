#include <mpi.h>
#include <iostream>
#include <unordered_map>
#include <memory>

#include "utilities.h"

enum class ProgramState { init, reading, stage1 };

enum class WorkerTypes {director, reader, worker};

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
        programState = ProgramState::init;
        directory = d;
        config = configuration;
    }

    virtual void run() { std::cout << "Base process run never meant to be called" << std::endl; }

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
        /* The director starts in */
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

        for (auto f : files)
            std::cout << "Reader " << readerNumber << " - " << f << std::endl;
    }

    void run() override
    {
        std::cout<<"I'm a reader! (" << worldId << "/" << worldSize << ")" << std::endl;
    }

private:
    std::vector<std::string> files;

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
        std::cout<<"I'm a worker! (" << worldId << "/" << worldSize << ")" << std::endl;
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

    // Construct the process directory
    std::shared_ptr<Directory> processDirectory(new Directory(world_size, config));

    Process *processWorker;

    switch (processDirectory->getProcessType(world_rank))
    {
        case WorkerTypes::director:
            processWorker = new Director(world_rank, world_size, config, processDirectory);
            break;
        case WorkerTypes::reader:
            processWorker = new Reader(world_rank, world_size, config, processDirectory);
            break;
        case WorkerTypes::worker:
            processWorker = new Worker(world_rank, world_size, config, processDirectory);
            break;
    }

    //Worker *processWorker = new Director(world_rank, world_size, ProgramState::init);
    processWorker->run();

    ///std::cout << processor_name << ", " << world_rank << "/" << world_size << std::endl;
    delete processWorker;

    MPI_Finalize();

}