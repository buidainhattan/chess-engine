#include "search_header.hpp"
#include "limits"

using namespace std;

Search::Search(ChessBoard &chessBoard, MoveGenerator &moveGenerator) : chessBoard(chessBoard), moveGenerator(moveGenerator)
{
}

Move Search::bestMove(int depth)
{
    vector<Move> legalMoves = moveGenerator.generateLegalMoves();
    if (legalMoves.empty())
    {
        return Move();
    }

    Move bestMove = legalMoves[0];
    int bestScore = (int)-INFINITY;

    for (Move move : legalMoves)
    {
        chessBoard.makeMove(move);
        int score = -search(depth - 1, (int)-INFINITY, (int)INFINITY);
        chessBoard.unMakeMove(move);

        if (score > bestScore)
        {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

int Search::search(int depth, int alpha, int beta)
{
    if (depth == 0)
        return chessBoard.currentState.materials;

    vector<Move> legalMoves = moveGenerator.generateLegalMoves();
    if (legalMoves.empty())
    {
        if (chessBoard.currentState.isInCheck)
            return (int)-INFINITY;
        return 0;
    }

    for (Move move : legalMoves)
    {
        chessBoard.makeMove(move);
        int evaluation = -search(depth - 1, -alpha, -beta);
        chessBoard.unMakeMove(move);

        if (evaluation >= beta)
            return beta;
        alpha = max(evaluation, alpha);
    }

    return alpha;
}