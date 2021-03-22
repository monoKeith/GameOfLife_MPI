#ifndef PROCESS_H
#define PROCESS_H

#include "piece.hpp"
#include "piece.hpp"
#include "mpi.h"

// Class for dist process
class Process
{
public:
    Process();
    ~Process();
    void initialize();
protected:
    bool INITIALIZED;
    int const ROOT_ID = 0;
    int PROCESSOR_ID;
    int PROCESSOR_COUNT;
    int N;
    int k;
    int m;
    Piece* piece;
};

#endif