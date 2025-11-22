#ifndef SEARCH_H
#define SEARCH_H

#include "../move_generator/move_generator_header.hpp"
#include "../evaluation/evaluation_header.hpp"
#include "../transposition_table/transposition_table_header.h"

class Search
{
public:
    Search(ChessBoard &chessBoard, MoveGenerator &moveGenerator);

    // Return best move of current position
    Move bestMove(int depth);

    // Search algorithm implement Negamax framework with Alpha-Beta pruning
    int search(int depth, int alpha, int beta);

private:
    ChessBoard &chessBoard;
    MoveGenerator &moveGenerator;

    Evaluation evaluation = Evaluation(chessBoard);
    TranspositionTable table;
};

#endif