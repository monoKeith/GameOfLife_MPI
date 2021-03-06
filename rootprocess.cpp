#include <iostream>
using namespace std;

#include "rootprocess.hpp"
#include <iomanip>
#include <algorithm>

RootProcess::RootProcess(char* argv[]){
    // Initialize class vars
    WORK_DISTRIBUTED = false;
    N = strtol(argv[1], NULL, 10);
    k = strtol(argv[2], NULL, 10);
    m = strtol(argv[3], NULL, 10);
    char* inputFileName = argv[4];

    inputFile = new FileHandle(N);
    inputFile->readFile(inputFileName);
    // inputFile.printFile();

    // Main process
    cout << endl;
    cout << "Processor Count: " << PROCESSOR_COUNT << endl;
}

RootProcess::~RootProcess(){
    delete inputFile;
    if (WORK_DISTRIBUTED){
        for (int i = 0; i < PROCESSOR_COUNT; ++i){
            delete workDistribution[i];
        }
        delete workDistribution;
    }
}


// Split the N * N square into x * x pieces, 1 piece for each processor.
// Assuming sqrt(processor_count) is a whole number, so x^2 = processor_count.
void RootProcess::splitSquares(int** workDistribution)
{
    // Split N into numPart parts
    int numPart = sqrt(PROCESSOR_COUNT);
    // For each part (square), the length of an edge is partLength
    int partLength = N / numPart;
    // An array of indices that the split happens, in total numPart + 1 lines.
    // First value always = 0, last value always = N - 1.
    int splitIndex[numPart][2];
    for (int i = 0; i < numPart; ++i){
        splitIndex[i][0] = i * partLength + 1;
        splitIndex[i][1] = (i + 1) * partLength;
    }
    splitIndex[0][0] = 0;
    splitIndex[numPart - 1][1] = N - 1;

    // Generate all possible combinations of splitIndex.
    // col/ row corresponds to slices of the map.
    for (int row = 0; row < numPart; ++row){
        for(int col = 0; col < numPart; ++col){
            int processorID = numPart * row + col;
            // X-span [0][1] & Y-span [2][3] for this processor
            workDistribution[processorID][0] = splitIndex[col][0];
            workDistribution[processorID][1] = splitIndex[col][1];
            workDistribution[processorID][2] = splitIndex[row][0];
            workDistribution[processorID][3] = splitIndex[row][1];
        }
    }
}


void RootProcess::distributeWork(){
    // Abort if already distributed.
    if (WORK_DISTRIBUTED) return;

    // int workDistribution[PROCESSOR_COUNT][4];
    workDistribution = new int* [PROCESSOR_COUNT];
    for (int i = 0; i < PROCESSOR_COUNT; ++i){
        workDistribution[i] = new int[4];
    }

    // Calculate indeces to slice the map into pieces.
    cout << "[" << fixed << setprecision(8) << currentTime() <<
            "] Splitting map..." << endl;

    splitSquares(workDistribution);

    // Distribute task to all processors, one by one.
    cout << "[" << fixed << setprecision(8) << currentTime() << 
            "] Distributing work..." << endl;

    int initVars[] = {N, k, m};
    MPI_Bcast(&initVars, 3, MPI_INT, ROOT_ID, MPI_COMM_WORLD);
    for (int processorID = 1; processorID < PROCESSOR_COUNT; ++processorID){
        // For each processor (except for root):
        // Send N, k, m
        MPI_Send(workDistribution[processorID], 4, MPI_INT, processorID, 0, MPI_COMM_WORLD);
        // Slice pipece for this processor
        int curLength = Piece::length(workDistribution[processorID]);
        char curPiece[curLength];
        inputFile->generatePiece(workDistribution[processorID], curPiece);
        // Send piece
        MPI_Send(curPiece, curLength, MPI_CHAR, processorID, 0, MPI_COMM_WORLD);
    }
    
    // Complete
    WORK_DISTRIBUTED = true;
}


void RootProcess::initialize(){
    // Abort if already initialized.
    if (INITIALIZED) return;

    // Initialize for root processor
    piece = new Piece(workDistribution[PROCESSOR_ID], PROCESSOR_ID, N);
    int curLength = Piece::length(workDistribution[PROCESSOR_ID]);
    char curPiece[curLength];
    inputFile->generatePiece(workDistribution[PROCESSOR_ID], curPiece);
    piece->initialize(curPiece);

    // set flag
    INITIALIZED = true;
}


// Synchronize external cells to every processor including root.
void RootProcess::syncExternalCells(){
    for (int processorID = 1; processorID < PROCESSOR_COUNT; ++processorID){
        // Generate external vals
        int curLength = Piece::externalLength(workDistribution[processorID]);
        char externalCells[curLength];
        inputFile->generateExternal(workDistribution[processorID], externalCells);
        // Distribute external vals
        MPI_Send(externalCells, curLength, MPI_CHAR, processorID, 0, MPI_COMM_WORLD);
    }

    // Generate externalCells for this processor
    int curLength = piece->externalLength();
    char externalCells[curLength];
    inputFile->generateExternal(workDistribution[PROCESSOR_ID], externalCells);
    piece->syncExternalCell(externalCells);
}


// Synchronize map from every processor including root.
void RootProcess::syncMap(){
    // Update map: root processor
    int curLength = piece->length();
    char curPiece[curLength];
    piece->getPiece(curPiece);
    inputFile->updatePiece(workDistribution[PROCESSOR_ID], curPiece);

    // Update map: other processors
    for (int processorID = 1; processorID < PROCESSOR_COUNT; ++processorID){
        // Allocate buffer to recv
        int curLength = Piece::length(workDistribution[processorID]);
        char curPiece[curLength];
        MPI_Recv(curPiece, curLength, MPI_CHAR, processorID, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        // Update map
        inputFile->updatePiece(workDistribution[processorID], curPiece);
    }
}


// Sync runtime
// Gather runtime from all processors
void RootProcess::syncRunTime(){
    double runTime = currentTime();
    double runTimes[PROCESSOR_COUNT];
    MPI_Gather(&runTime, 1, MPI_DOUBLE, runTimes, 1, MPI_DOUBLE, ROOT_ID, MPI_COMM_WORLD);
    // Sort
    sort(runTimes, runTimes + PROCESSOR_COUNT);
    cout << endl << " Total Runtime: Fastest = " << runTimes[0] << 
                    " Slowest = " << runTimes[PROCESSOR_COUNT - 1] << endl;
}


void RootProcess::run(){
    distributeWork();
    initialize();
    // Wait for everyone
    MPI_Barrier(MPI_COMM_WORLD);
    // Print work range
    piece->printWorkRange();
    // Run
    int mRemain = m;
    for (int i = 1; i <= k; ++i){
        iterate();
        cout << "[" << fixed << setprecision(8) << currentTime() << 
                "] Finished Iteration # " << i << endl;
        // For each m-th evolutionary step, print.
        if (--mRemain == 0){
            char filenameBuf[64];
            sprintf(filenameBuf, "iteration_%d.txt", i);
            inputFile->saveFile(filenameBuf);
            mRemain = m; 
        }
    }
    // Synchronize runtime
    syncRunTime();
}