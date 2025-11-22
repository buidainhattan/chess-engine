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
    int originalAlpha = alpha;

    // [CHANGE 2] Calculate the Zobrist Hash of the current board
    // (Assuming your board has a method or variable for this)
    U64 key = chessBoard.currentState.zobristKey;

    // [CHANGE 3] PROBE THE TABLE
    // Try to retrieve a score and a "best move" from a previous search
    int ttScore;
    Move ttMove = Move(); // Placeholder

    // Note: probe should return TRUE only if the depth is sufficient AND
    // the score is useful (cut off based on current alpha/beta)
    if (table.probe(key, depth, alpha, beta, ttScore, ttMove))
    {
        return ttScore; // Return immediately!
    }

    if (depth == 0)
        return evaluation.evaluate();

    vector<Move> legalMoves = moveGenerator.generateLegalMoves();;
    if (ttMove != Move()) {
        legalMoves.push_back(ttMove);
    }
    if (legalMoves.empty())
    {
        if (chessBoard.currentState.isInCheck)
            return NEGATIVE_INFINITY;
        return 0;
    }

    Move bestMoveOfPosition = Move();
    for (Move move : legalMoves)
    {
        chessBoard.makeMove(move);
        int evaluation = -search(depth - 1, -beta, -alpha);
        chessBoard.unMakeMove(move);

        if (evaluation >= beta)
        {
            table.storeEntry(key, beta, depth, LOWER_BOUND, move);
            return beta;
        }
        if (evaluation > alpha) {
            alpha = evaluation;
            bestMoveOfPosition = move;
        }
    }
    NodeFlag flag;
    if (alpha <= originalAlpha) {
        flag = UPPER_BOUND;
    } else {
        flag = EXACT;
    }
    table.storeEntry(key, alpha, depth, flag, bestMoveOfPosition);

    return alpha;
}