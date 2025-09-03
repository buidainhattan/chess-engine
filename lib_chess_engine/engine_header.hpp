#ifndef ENGINE_H
#define ENGINE_H

#include "move_generator/move_generator_header.hpp"
#include "search/search_header.hpp"

class Engine
{
public:
    ChessBoard chessBoard;
    MoveGenerator moveGenerator = MoveGenerator(chessBoard);
    Search search = Search(chessBoard, moveGenerator);
};

#endif