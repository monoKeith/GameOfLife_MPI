#ifndef PROCESS_H
#define PROCESS_H

#include "piece.hpp"

class Process
{
public:
    Process();
    ~Process();

private:
    Piece* piece;
};

#endif