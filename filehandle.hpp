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
    void saveFile(char* filename);
    void generatePiece(int workRange[], char curPiece[]);
    void generateExternal(int workRange[], char extCells[]);
    void updatePiece(int workRangep[], char curPiece[]);
private:
    // N = dimension of the square map.
    int N;
    bool used;
};

#endif