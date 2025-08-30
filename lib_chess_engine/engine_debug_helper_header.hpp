#ifndef PERFT_HELPER_H
#define PERFT_HELPER_H

#include "chess_board_header.hpp"
#include "engine_header.hpp"

// Perft function to test legal moves generator if it working porperly
long long perftBulkCount(int depth, ChessBoard &chessBoard, Engine &engine);

void perftBulkCountDivide(int depth, ChessBoard &chessBoard, Engine &engine);

long long perft(int depth,
                long long &capturesCount,
                long long &enpassantCount,
                long long &castlingCount,
                long long &promotionsCount,
                long long &checksCount,
                long long &checkmatesCount,
                int previousFrom,
                int previousTo,
                ChessBoard &chessBoard,
                Engine &engine);

void perftDivide(int depth, ChessBoard &chessBoard, Engine &engine);

#endif