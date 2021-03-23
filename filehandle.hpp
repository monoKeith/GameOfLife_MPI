#ifndef FILEHANDLE_H
#define FILEHANDLE_H

#include <fstream>

class FileHandle {
public:
    FileHandle(int n);
    ~FileHandle();
    bool** val;
    void readFile(char* inputFile);
    void printFile();
    void generatePiece(int workRange[], char curPiece[]);
    void generateExternal(int workRange[], char extCells[]);

private:
    // N = dimension of the square map.
    int N;
    bool used;
};

#endif