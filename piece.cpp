#include <iostream>
using namespace std;

#include "piece.hpp"

/*
    workRange[]: {x_min, x_max, y_min, y_max}, inclusive.
    processorID: PROCESSOR_ID
    upperBound:  N of the original map, exclusive.
*/
Piece::Piece(int workRange[], int processorID, int N){
    // Initialize class vars
    PROCESSOR_ID = processorID;
    workRange_X[0] = workRange[0];
    workRange_X[1] = workRange[1];
    workRange_Y[0] = workRange[2];
    workRange_Y[1] = workRange[3];
    INITIALIZED = false;
    lowerBound = 0;
    upperBound = N - 1;
    // Print work range of this processor
    cout << "Processor #" << PROCESSOR_ID << 
        "\t X: " << workRange_X[0] << " - " << workRange_X[1] << 
        "\t Y: " << workRange_Y[0] << " - " << workRange_Y[1] << endl;
}


Piece::~Piece(){
    if (INITIALIZED){
        for (int y = 0; y < length_Y; ++y){
            delete piece[y];
        }
        delete piece;
    }
}


// Populate (fill in) current piece with an array.
void Piece::initialize(char input[]){
    // Abort if already initialized.
    if (INITIALIZED) return;
    // Initialize size
    length_X = workRange_X[1] - workRange_X[0] + 1;
    length_Y = workRange_Y[1] - workRange_Y[0] + 1;
    // Initialize piece
    int curIndex = 0;
    piece = (bool**) malloc(length_Y * sizeof(bool*));
    for (int y = 0; y < length_Y; ++y){
        piece[y] = (bool*) malloc(length_X * sizeof(bool));
        for (int x = 0; x < length_X; ++x){
            piece[y][x] = (bool) input[curIndex++];
        }
    }
    // Set initialized flag
    INITIALIZED = true;
}


// Export current piece into an array. Same format as initialize.
void Piece::getPiece(char result[]){
    // Export
    int curIndex = 0;
    for(int y = 0; y < length_Y; ++y){
        for(int x = 0; x < length_X; ++x){
            result[curIndex++] = piece[y][x];
        }
    }
}


// Print current piece to terminal.
void Piece::printPiece(){
    for(int y = 0; y < length_Y; ++y){
        for(int x = 0; x < length_X; ++x){
            cout << (piece[y][x] ? '*' : ' ');
        }
        cout << endl;
    }
}


// Return the number of cells in this piece.
int Piece::length(){
    if (INITIALIZED) {
        return length_X * length_Y;
    }
    return 0;
}


// Static method to calculate total length of a piece, by using workRange[].
int Piece::length(int workRange[]){
    int length_X = workRange[1] - workRange[0] + 1;
    int length_Y = workRange[3] - workRange[2] + 1;
    return length_X * length_Y;
}