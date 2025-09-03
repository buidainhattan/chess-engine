#ifndef EVALUATION_H
#define EVALUATION_H

#include "../chess_board/chess_board_header.hpp"

class Evaluation
{
public:
    Evaluation(ChessBoard &chessBoard);

    int evaluate();

    std::vector<Move> orderMoves(std::vector<Move> legalMoves);

private:
    ChessBoard &chessBoard;

    // Count materials value
    int countMaterial(int pieceType, int numberOfPieces);

    // Score pieces position
    int scorePosition(int pieceType, U64 pieceBitBoard, Color side);

    // Flip index of black to white
    int getFlippedIndex(int square);
};

#endif