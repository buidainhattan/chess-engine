#ifndef ENGINE_H
#define ENGINE_H

#include <fcntl.h>      // for _setmode
#include <io.h>         // for _setmode
#include <stdio.h>      // for _fileno

#include <iostream>     // for std::wcout

#include "move_generator/move_generator_header.hpp"
#include "search/search_header.hpp"

class Engine
{
public:
    void reset();
    void loadFromFEN(std::string FEN);
    Move convertStringToMove(const std::string &moveString);
    void prettyPrintBitboard();
    BoardState getCurrentBoardState();
    Color getCurrentSide();
    std::vector<Move> getLegalMoves();
    void makeMove(Move move);
    void unMakeMove(Move move);
    Move getBestMove(int depth);
    int getKingSquare(Color kingSide);
    bool isInCheck(Color kingSide);
    void printRepetitionTracker();
    bool isRepetition();
    std::string disambiguating(Color sideToMove, Move move);

private:
    bool botEnabled = false;
    Color botSide = color_NB;
    ChessBoard chessBoard = ChessBoard();
    MoveGenerator moveGenerator = MoveGenerator(chessBoard);
    Search search = Search(chessBoard, moveGenerator);

    U64 getRankBitboard(const int square);
    U64 getFileBitboard(const int square);
};

#endif