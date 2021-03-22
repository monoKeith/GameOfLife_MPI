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
    // Identify position
    pos_X = HorizontalPosition::middle;
    if (workRange_X[0] == lowerBound)   pos_X = HorizontalPosition::left;
    if (workRange_X[1] == upperBound)   pos_X = HorizontalPosition::right;
    pos_Y = VerticalPosition::middle;
    if (workRange_Y[0] == lowerBound)   pos_Y = VerticalPosition::top;
    if (workRange_Y[1] == upperBound)   pos_Y = VerticalPosition::bottom;
    // Print work range of this processor
    cout << "Processor #" << PROCESSOR_ID << 
        "\t X: " << workRange_X[0] << " - " << workRange_X[1] << 
        "\t Y: " << workRange_Y[0] << " - " << workRange_Y[1] << endl;
}


Piece::~Piece(){
    if (INITIALIZED){
        deletePiece(piece);
    }
}


void Piece::deletePiece(bool** p){
    for (int y = 0; y < length_Y; ++y){
        delete p[y];
    }
    delete p;
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


// Count neighbour for cell.
// targetY & targetX corresponds to internal indices. (1 to length - 1)
int Piece::countNeighbor(int targetY, int targetX){
    int count = 0;
    for (int y = targetY - 1; y <= targetY + 1; ++y){
        for (int x = targetX - 1; x <= targetX + 1; ++x){
            if (y >= 0 && y < length_Y && x >= 0 && x < length_X){
                // Internal cell
                if (piece[y][x]) count += 1;
            }else{
                // External cell
                // Translate coordinates to real coordinates
                int real_X = workRange_X[0] + x;
                int real_Y = workRange_Y[0] + y;
                // Access external cell

            }
        }
    }
    // If target itself is true, subtract 1 from result.
    if (piece[targetY][targetX]) count -= 1;
    return count;
}


// calculate next iteration
void Piece::iterate(){
    // Allocate new piece
    bool** nextPiece = (bool**) malloc(length_Y * sizeof(bool*));
    for (int y = 0; y < length_Y; ++y){
        piece[y] = (bool*) malloc(length_X * sizeof(bool));
    }
    // Iterate all cells
    for (int y = 1; y < length_Y - 1; ++y){
        for (int x = 1; x < length_X - 1; ++x){
            // calculate amount of neighbour
            int neighbour = countNeighbor(y, x);
            if (neighbour >= 2 && neighbour <= 3){
                // survives
                nextPiece[y][x] = piece[y][x];
            }
            if (neighbour >= 4){
                // dies from overpopulation
                nextPiece[y][x] = false;
            }
            if (neighbour <= 1){
                // dies from isolation
                nextPiece[y][x] = false;
            }
            if (neighbour == 3){
                // give birth
                nextPiece[y][x] = true;
            }
        }
    }
    // Update piece
    deletePiece(piece);
    piece = nextPiece;
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