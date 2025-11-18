#include "zobrist_hash_generator_header.h"

using namespace std;

ZobristHashGenerator::ZobristHashGenerator()
{
    initialize();
}

U64 ZobristHashGenerator::getZobristKey(U64 (&pieceBitboardsByColor)[6], BoardState &state)
{
    Color sideToMove = state.sideToMove;
    U64 zobristKey = ULLONG_MAX ^ (sideToMove * zobristKeys[768]);
    for (int pieceType = 0; pieceType < pieceType_NB; pieceType++) {
        U64 pieceBitboard = pieceBitboardsByColor[pieceType];
        while (pieceBitboard) {
            int pieceSquareIndex = bitScanForward(pieceBitboard);
            popLSB(pieceBitboard);
            int keyIndex = (sideToMove * 6 + pieceType) * 64 + pieceSquareIndex;
            zobristKey ^= zobristKeys[keyIndex];
        }
    }
    if (state.castlingRights) {
        zobristKey ^= (state.canCastleKingside(white) * zobristKeys[769]) ^ (state.canCastleQueenside(white) * zobristKeys[770]);
        zobristKey ^= (state.canCastleKingside(black) * zobristKeys[771]) ^ (state.canCastleQueenside(black) * zobristKeys[772]);
    }

    return zobristKey;
}

void ZobristHashGenerator::initialize()
{
    for (int i = 0; i < ZOBRIST_HASH_SIZE; i++) {
        zobristKeys[i] = keyPNG();
    }
}
