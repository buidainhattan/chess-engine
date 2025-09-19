#ifndef ENGINE_H
#define ENGINE_H

#include "move_generator/move_generator_header.hpp"
#include "search/search_header.hpp"

class EngineLite
{
public:
    ChessBoard chessBoard = ChessBoard();
    MoveGenerator moveGenerator = MoveGenerator(chessBoard);

    Move convertStringToMove(const std::string &moveString);
};

class Engine : public EngineLite
{
public:
    Search search = Search(chessBoard, moveGenerator);
};

#endif