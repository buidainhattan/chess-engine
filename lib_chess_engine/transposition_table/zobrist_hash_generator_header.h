#ifndef ZOBRIST_HASH_GENERATOR_H
#define ZOBRIST_HASH_GENERATOR_H

#include <random>

#include "../helpers/bitboard_helper_header.hpp"

class ZobristHashGenerator
{
public:
    ZobristHashGenerator();
    U64 getZobristKey(U64 (&pieceBitboardsByColor)[6], BoardState &state);

private:
    U64 zobristKeys[ZOBRIST_HASH_SIZE] = {0};
    std::mt19937_64 keyPNG = std::mt19937_64(ZOBRIST_SEED);

    void initialize();
};

#endif