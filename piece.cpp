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
        delete externalCells;
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
    // Allocate & initialize piece
    int curIndex = 0;
    piece = (bool**) malloc(length_Y * sizeof(bool*));
    for (int y = 0; y < length_Y; ++y){
        piece[y] = (bool*) malloc(length_X * sizeof(bool));
        for (int x = 0; x < length_X; ++x){
            piece[y][x] = (bool) input[curIndex++];
        }
    }
    // Allocate external cells
    externalCells = new bool[externalLength()];
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
                if (getExternalCell(y, x)) count += 1;
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
        nextPiece[y] = (bool*) malloc(length_X * sizeof(bool));
    }
    // Iterate all cells
    for (int y = 0; y < length_Y; ++y){
        for (int x = 0; x < length_X; ++x){
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


void Piece::syncExternalCell(char input[]){
    int extLength = externalLength();
    for (int i = 0; i < extLength; ++i){
        externalCells[i] = input[i];
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


// Calculate the length of external values
int Piece::externalLength(){
    /* 
        4 edges + 4 cells
        +---x---+
        |       |
        y       y
        |       |
        +---x---+
    */
    return 2 * (length_X + length_Y) + 4;
}


int Piece::externalLength(int workRange[]){
    int length_X = workRange[1] - workRange[0] + 1;
    int length_Y = workRange[3] - workRange[2] + 1;
    return 2 * (length_X + length_Y) + 4;
}

// Calculate the length of boundary values
int Piece::boundaryLength(){
    return 2 * (length_X + length_Y) - 4;
}


// Access value of an external cell
// y & x corresponds to internal indices
bool Piece::getExternalCell(int y, int x){
    if (y >= 0 && y < length_Y && x >= 0 && x < length_X){
        // Internal cell, shouldn't use this function but its supported.
        // Very likely to happen when index is wriong.
        cout << "WARNING! Unexpected getExternalCell() call." << endl;
        return piece[y][x];
    }
    // Access external cell array
    if (y == -1 && x >= -1 && x <= length_X){
        // First row
        int startIndex = 1;
        return externalCells[startIndex + x];
    }
    if (y >= 0 && y <= length_Y - 1){
        // Middle rows
        int startIndex = (length_X + 2) + (2 * y);
        if (x == -1) {
            // First column
            return externalCells[startIndex];
        }
        if (x == length_X){
            // Last column (out of bound by 1)
            return externalCells[startIndex + 1];
        }  
    }
    if (y == length_Y && x >= -1 && x <= length_X){
        // Last row (out of bound by 1)
        int startIndex = (length_X + 3) + (2 * length_Y);
        return externalCells[startIndex + x];
    }
    // If none match, return false.
    return false;
}


