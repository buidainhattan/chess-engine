#include "zobrist_hash_generator_header.h"

using namespace std;

ZobristHashGenerator::ZobristHashGenerator()
{
    initialize();
}

void ZobristHashGenerator::setInitialZobristKey(U64 (&pieceBitboardsByColor)[2][6], BoardState &state)
{
    U64 zobristKey = ULLONG_MAX ^ (state.sideToMove * zobristKeys[768]);
    for (int pieceType = 0; pieceType < pieceType_NB; pieceType++)
    {
        U64 whitePieceBitboard = pieceBitboardsByColor[white][pieceType];
        while (whitePieceBitboard)
        {
            int pieceSquareIndex = bitScanForward(whitePieceBitboard);
            popLSB(whitePieceBitboard);
            int keyIndex = pieceType * 64 + pieceSquareIndex;
            zobristKey ^= zobristKeys[keyIndex];
        }
        U64 blackPieceBitboard = pieceBitboardsByColor[black][pieceType];
        while (blackPieceBitboard)
        {
            int pieceSquareIndex = bitScanForward(blackPieceBitboard);
            popLSB(blackPieceBitboard);
            int keyIndex = (6 + pieceType) * 64 + pieceSquareIndex;
            zobristKey ^= zobristKeys[keyIndex];
        }
    }
    if (state.castlingRights)
    {
        zobristKey ^= ((state.canCastleKingside(white) * zobristKeys[769]) ^ (state.canCastleQueenside(white) * zobristKeys[770]));
        zobristKey ^= ((state.canCastleKingside(black) * zobristKeys[771]) ^ (state.canCastleQueenside(black) * zobristKeys[772]));
    }
    if (state.enPassantSquare != -1)
    {
        zobristKey ^= zobristKeys[773 + (state.enPassantSquare % 8)];
    }

    state.zobristKey = zobristKey;
}

void ZobristHashGenerator::updateZobristKey(BoardState &previousState, BoardState &currentState, PieceType &movingPiece, Move &move)
{
    Color sideToMove = currentState.sideToMove;
    U64 zobristKey = currentState.zobristKey;
    int movingKeyIndex = (sideToMove * 6 + movingPiece) * 64;
    zobristKey ^= (zobristKeys[movingKeyIndex + move.from()] ^ zobristKeys[movingKeyIndex + move.to()]);
    if (currentState.captured != pieceType_NB)
    {
        int capturedKeyIndex = (!sideToMove * 6 + currentState.captured) * 64;
        zobristKey ^= zobristKeys[capturedKeyIndex + move.to()];
    }
    zobristKey ^= (!sideToMove * zobristKeys[768]);
    if (currentState.castlingRights != previousState.castlingRights)
    {
        int changedRights = currentState.castlingRights ^ previousState.castlingRights;
        if (changedRights & 8)
        {
            zobristKey ^= zobristKeys[769];
        }
        if (changedRights & 4)
        {
            zobristKey ^= zobristKeys[770];
        }
        if (changedRights & 2)
        {
            zobristKey ^= zobristKeys[771];
        }
        if (changedRights & 1)
        {
            zobristKey ^= zobristKeys[772];
        }
        previousState.castlingRights = currentState.castlingRights;
    }
    if (previousState.enPassantSquare != -1)
    {
        zobristKey ^= zobristKeys[773 + previousState.enPassantSquare % 8];
    }
    if (currentState.enPassantSquare != -1)
    {
        zobristKey ^= zobristKeys[773 + currentState.enPassantSquare % 8];
    }

    currentState.zobristKey = zobristKey;
}

void ZobristHashGenerator::initialize()
{
    for (int i = 0; i < ZOBRIST_HASH_SIZE; i++)
    {
        zobristKeys[i] = keyPNG();
    }
}
