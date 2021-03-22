#ifndef PIECE_H
#define PIECE_H

class Piece {
public:
    Piece(int workRange[], int processorID, int N);
    ~Piece();
    void initialize(char input[]);
    void printPiece();
    void getPiece(char result[]);
    void iterate();
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
    // Ptr to current piece.
    bool** piece;
    // Piece position on the original map
    enum class HorizontalPosition   { left, middle, right };
    enum class VerticalPosition     { top, middle, bottom };
    HorizontalPosition  pos_X;
    VerticalPosition    pos_Y;
    // Clean memory
    void deletePiece(bool** p);
    // Count neighbors
    int countNeighbor(int y, int x);
    
};

#endif