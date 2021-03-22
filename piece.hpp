#ifndef PIECE_H
#define PIECE_H

class Piece {
public:
    Piece(int workRange[], int processorID, int N);
    ~Piece();
    void initialize(char input[]);
    void printPiece();
    void getPiece(char result[]);
    int length();
    static int length(int workRange[]);
private:
    int PROCESSOR_ID;
    int workRange_X[2];
    int workRange_Y[2];
    int length_X;
    int length_Y;
    // Inclusive Bounds of the original map, used for edge detection.
    int lowerBound;
    int upperBound;

    bool INITIALIZED;
    bool** piece;
};

#endif