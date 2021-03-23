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
    void splitSquares(int** workDistribution);
    void initialize() override;
    void syncExternalCells() override;
    void syncMap() override;
    void syncRunTime() override;
};

#endif