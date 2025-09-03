#include "search_header.hpp"

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
    int bestScore = NEGATIVE_INFINITY;

    for (Move move : legalMoves)
    {
        chessBoard.makeMove(move);
        int score = -search(depth - 1, NEGATIVE_INFINITY, POSITIVE_INFINITY);
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
        return evaluation.evaluate();

    vector<Move> legalMoves = moveGenerator.generateLegalMoves();
    if (legalMoves.empty())
    {
        if (chessBoard.currentState.isInCheck)
            return NEGATIVE_INFINITY;
        return 0;
    }

    for (Move move : legalMoves)
    {
        chessBoard.makeMove(move);
        int evaluation = -search(depth - 1, -beta, -alpha);
        chessBoard.unMakeMove(move);

        if (evaluation >= beta)
        {
            return beta;
        }
        alpha = max(alpha, evaluation);
    }

    return alpha;
}