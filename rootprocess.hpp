#ifndef ROOTPROCESS_H
#define ROOTPROCESS_H

#include "filehandle.hpp"
#include "process.hpp"
#include <math.h>

class RootProcess: public Process {
public:
    RootProcess(char* argv[]);
    ~RootProcess();
    void run();
private:
    FileHandle* inputFile;
    bool WORK_DISTRIBUTED;
    int** workDistribution;
    void distributeWork();
    void initialize();
    void iterate();
    void splitSquares(int** workDistribution);
    void syncExternalCells();
    void syncMap();
};

#endif