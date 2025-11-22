#ifndef TRANSPOSITION_TABLE_H
#define TRANSPOSITION_TABLE_H

#include "../generals/all_enums_header.hpp"

class TranspositionTable
{
public:
    void storeEntry(U64 zobristKey, int score, int depth, NodeFlag flag, Move bestMove);
    bool probe(U64 zobristKey, int depth, int alpha, int beta, int &score, Move &bestMove);

private:
    TranspositionEntry *table = new TranspositionEntry[TABLE_SIZE];

    U64 getIndex(U64 zobristKey);
};

#endif