#ifndef SEARCH_H
#define SEARCH_H

#include "../helpers/bitboard_helper_header.hpp"
#include "../chess_board/chess_board_header.hpp"
#include "../move_generator/move_generator_header.hpp"

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
};

#endif