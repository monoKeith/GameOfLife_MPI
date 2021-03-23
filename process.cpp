#include <iostream>
using namespace std;

#include "process.hpp"

Process::Process(){
    INITIALIZED = false;
    PROCESSOR_COUNT = MPI::COMM_WORLD.Get_size();
    PROCESSOR_ID = MPI::COMM_WORLD.Get_rank();
}

Process::~Process(){
    if (INITIALIZED){
        delete piece;
    }
}


void Process::initialize(){
    // Abort if already initialized.
    if (INITIALIZED) return;

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
    piece = new Piece(workRange, PROCESSOR_ID, N);
    int curLength = Piece::length(workRange);
    char curPiece[curLength];
    MPI_Recv(curPiece, curLength, MPI_CHAR, ROOT_ID, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    piece->initialize(curPiece);

    // Set flag
    INITIALIZED = true;
}


// Calculate and generate next iteration.
void Process::iterate(){
    // Wait for everyone
    MPI_Barrier(MPI_COMM_WORLD);

    // Sync External Cells
    int curLength = piece->externalLength();
    char curExternalCell[curLength];
    MPI_Recv(curExternalCell, curLength, MPI_CHAR, ROOT_ID, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    piece->syncExternalCell(curExternalCell);

    // Iterate
    piece->iterate();

    // Wait for others to finish
    MPI_Barrier(MPI_COMM_WORLD);

    // Send current piece to root
    curLength = piece->length();
    char curPiece[curLength];
    piece->getPiece(curPiece);
    MPI_Send(curPiece, curLength, MPI_CHAR, ROOT_ID, 0, MPI_COMM_WORLD);
}

void Process::run(){
    initialize();
    for (int i = 0; i < k; ++i){
        iterate();
    }
}