
#include "filehandle.hpp"
#include "piece.hpp"
#include "process.hpp"
#include "mpi.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <math.h>
using namespace std;

// Global vars
const int ROOT_ID = 0;
int N, k, m;
int PROCESSOR_ID;
int PROCESSOR_COUNT;


// Split the N * N square into x * x pieces, 1 piece for each processor.
// Assuming sqrt(processor_count) is a whole number, so x^2 = processor_count.
int splitSquares(int workDistribution[][4])
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

    // Print work split detail
    // for (int i = 0; i < PROCESSOR_COUNT; ++i){
    //     int* wd = workDistribution[i];
    //     cout << "Processor #" << i << 
    //             "\t X: " << wd[0] << " - " << wd[1] << 
    //             "\t Y: " << wd[2] << " - " << wd[3] << endl;
    // }
}


// Distribute work to every processor. 
// !!!!! ONLY used by root host.
void distributeWork(FileHandle* inputFile){
    int workDistribution[PROCESSOR_COUNT][4];
    // Calculate indeces to slice the map into pieces.
    cout << "Splitting work..." << endl;

    splitSquares(workDistribution);

    cout << "Distributing work..." << endl;

    // Distribute task to all processors, one by one.
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
    
    cout << "Work distribution complete!" << endl;

    // Initialize for root processor
    Piece piece(workDistribution[PROCESSOR_ID], PROCESSOR_ID, N);
    int curLength = Piece::length(workDistribution[PROCESSOR_ID]);
    char curPiece[curLength];
    inputFile->generatePiece(workDistribution[PROCESSOR_ID], curPiece);
    piece.initialize(curPiece);

}


void run_host(FileHandle* inputFile)
{
    distributeWork(inputFile);

}


void run_dist()
{
    




    // Initialize N, k, m.
    int initVars[3];
    MPI_Bcast(&initVars, 3, MPI_INT, ROOT_ID, MPI_COMM_WORLD);
    N = initVars[0];
    k = initVars[1];
    m = initVars[2];
    // Initialize work distribution.
    int workRange[4];
    MPI_Recv(workRange, 4, MPI_INT, ROOT_ID, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    
    // Initialize piece & receive piece
    Piece piece(workRange, PROCESSOR_ID, N);
    int curLength = Piece::length(workRange);
    char curPiece[curLength];
    MPI_Recv(curPiece, curLength, MPI_CHAR, ROOT_ID, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    piece.initialize(curPiece);

}


int main(int argc, char* argv[])
{
    double wtime = MPI::Wtime();

    //  Initialize MPI.
    MPI::Init(argc, argv);

    // Number of processes & individual process ID.
    PROCESSOR_COUNT = MPI::COMM_WORLD.Get_size();
    PROCESSOR_ID = MPI::COMM_WORLD.Get_rank();

    if (PROCESSOR_ID == 0) {
        // Host: Read file.
        if (argc != 5) {
            cout << "ARGC = " << argc << endl;
            cout << "Usage: run <N> <k> <m> <file name>" << endl;
            MPI_Abort(MPI::COMM_WORLD, 1);
        }

        // Check if process count supported.
        float numPart = sqrt(PROCESSOR_COUNT);
        if (ceil(numPart) > numPart) {
            cout << "Processor count unsupported! Should be a square! (e.g. 2, 4, 8, 16, 32)" << endl;
            MPI_Abort(MPI::COMM_WORLD, 1);
        }

        N = strtol(argv[1], NULL, 10);
        k = strtol(argv[2], NULL, 10);
        m = strtol(argv[3], NULL, 10);
        char* inputFileName = argv[4];

        FileHandle inputFile(N);
        inputFile.readFile(inputFileName);
        // inputFile.printFile();

        // Main process
        cout << endl;
        cout << "Processor Count: " << PROCESSOR_COUNT << endl;

        // Run host
        run_host(&inputFile);
    } else {
        // Run dist
        run_dist();
    }

    // Terminate MPI.
    MPI::Finalize();
    wtime = MPI::Wtime() - wtime;

    if (PROCESSOR_ID == 0) {
        cout << "  Elapsed wall clock time = " << wtime << " seconds.\n";
    }

    return 0;
}
