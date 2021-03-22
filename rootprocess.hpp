#ifndef ROOTPROCESS_H
#define ROOTPROCESS_H

#include "filehandle.hpp"
#include "process.hpp"
#include <math.h>

class RootProcess: public Process {
public:
    RootProcess(char* argv[]);
    ~RootProcess();
    void distributeWork();
    void initialize();
private:
    FileHandle* inputFile;
    bool WORK_DISTRIBUTED;
    int** workDistribution;
    void splitSquares(int** workDistribution);
};

#endif