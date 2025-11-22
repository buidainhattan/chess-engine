#include "transposition_table_header.h"

using namespace std;

void TranspositionTable::storeEntry(U64 zobristKey, int score, int depth, NodeFlag flag, Move bestMove)
{
    TranspositionEntry &entry = table[getIndex(zobristKey)];

    if (entry.zobristKey == 0 || depth >= entry.depth)
    {
        entry.zobristKey = zobristKey;
        entry.score = score;
        entry.depth = depth;
        entry.flag = flag;
        entry.bestMove = bestMove;
    }
}

bool TranspositionTable::probe(U64 zobristKey, int depth, int alpha, int beta, int &score, Move &bestMove)
{
    TranspositionEntry &entry = table[getIndex(zobristKey)];

    if (entry.zobristKey == zobristKey)
    {
        bestMove = entry.bestMove;

        if (entry.depth >= depth) {
            if (entry.flag == EXACT) {
                score = entry.score;
                return true;
            }
            if ( entry.flag == UPPER_BOUND && entry.score <= alpha) {
                score = alpha;
                return true;
            }
            if ( entry.flag == LOWER_BOUND && entry.score >= beta) {
                score = beta;
                return true;
            }
        }
    }
    return false;
}

U64 TranspositionTable::getIndex(U64 zobristKey)
{
    return zobristKey & (TABLE_SIZE - 1);
}