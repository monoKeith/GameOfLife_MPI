#include <iostream>
using namespace std;

#include "filehandle.hpp"

FileHandle::FileHandle(int n)
{
    N = n;
    used = false;
}

FileHandle::~FileHandle()
{
    if (used) {
        for (int i = 0; i < N; ++i) {
            delete val[i];
        }
        delete val;
    }
}

void FileHandle::readFile(char* inputFile)
{
    // if file already read, abort.
    if (used)
        return;

    cout << " Reading input file: " << inputFile << endl;
    ifstream infile(inputFile);

    int curInt;
    bool** orig = (bool**)malloc(N * (sizeof(bool*)));

    // File format: multiple lines of integer, no space.
    // Read file and initialize array orig.
    for (int y = 0; y < N; ++y) {
        orig[y] = (bool*)malloc(N * (sizeof(bool)));
        char thisLine[N];
        infile >> thisLine;
        for (int x = 0; x < N; ++x) {
            orig[y][x] = thisLine[x] == '0' ? false : true;
        }
    }

    val = orig;
    // set status to already read.
    used = true;
}

void FileHandle::printFile()
{
    // abort if the file is not read
    if (!used)
        return;

    for (int y = 0; y < N; ++y) {
        for (int x = 0; x < N; ++x) {
            cout << (val[y][x] ? '*' : ' ');
        }
        cout << endl;
    }
}

void FileHandle::generatePiece(int workRange[], char result[])
{
    // Export
    int curIndex = 0;
    for (int y = workRange[2]; y <= workRange[3]; ++y){
        for (int x = workRange[0]; x <= workRange[1]; ++x){
            result[curIndex++] = val[y][x];
        }
    }
}