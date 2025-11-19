#ifndef ZOBRIST_HASH_GENERATOR_H
#define ZOBRIST_HASH_GENERATOR_H

#include <random>

#include "../helpers/bitboard_helper_header.hpp"

class ZobristHashGenerator
{
public:
    ZobristHashGenerator();
    void setInitialZobristKey(U64 (&pieceBitboardsByColor)[2][6], BoardState &state);
    void updateZobristKey(BoardState &previousState, BoardState &currentState, PieceType &movingPiece, Move &move);

private:
    U64 zobristKeys[ZOBRIST_HASH_SIZE] = {0};
    std::mt19937_64 keyPNG = std::mt19937_64(ZOBRIST_SEED);

    void initialize();
};

#endif