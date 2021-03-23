#ifndef PIECE_H
#define PIECE_H

class Piece {
public:
    Piece(int workRange[], int processorID, int N);
    ~Piece();
    // Fill in this piece with an array. 
    // Dimension already defined in constructor.
    void initialize(char input[]);
    // Print to console.
    void printPiece();
    // Returns current piece to result[].
    // result[] needs to be allocated with correct length. 
    void getPiece(char result[]);
    // Calculate next iteration.
    void iterate();
    // Get length of current piece.
    int length();
    // Static method to calculate length of piece.
    static int length(int workRange[]);
    // Get size of external values
    int externalLength();
    // Static method to calculate length of external var.
    static int externalLength(int workRange[]);
    // Get size of boundary values
    int boundaryLength();
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
    // Ptr to external cells arr.
    bool* externalCells;
    // Piece position on the original map
    enum class HorizontalPosition   { left, middle, right };
    enum class VerticalPosition     { top, middle, bottom };
    HorizontalPosition  pos_X;
    VerticalPosition    pos_Y;
    // Clean memory
    void deletePiece(bool** p);
    // Count neighbors
    int countNeighbor(int y, int x);
    // Get value of an external cell
    bool getExternalCell(int y, int x);
};

#endif