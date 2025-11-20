#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <sstream>
#include <cctype>
#include <stack>
#include <unordered_map>

#include "../helpers/bitboard_helper_header.hpp"
#include "../transposition_table/zobrist_hash_generator_header.h"

class ChessBoard
{
public:
    ChessBoard();

    U64 pieceBitboards[color_NB][pieceType_NB] = {0ULL};
    U64 pieceColorBitboards[color_NB] = {0ULL};
    PieceType pieceAt[square_NB];

    U64 allPieces, empty;

    std::stack<BoardState> stateHistory = {};
    BoardState currentState;
    std::unordered_map<U64, int> repetitionTracker = {};
    bool isRepetition = false;

    void loadFromFEN(std::string FEN);
    void FENStringParser(std::string FEN);
    std::string FENStringCrafter();
    void initialOccupancy();

    void switchActiveSide();

    void makeMove(Move move);
    void unMakeMove(Move move);

    template <PieceType pieceType>
    U64 const getAllPiecesOfType()
    {
        if (pieceType == pawn)
            return pieceBitboards[white][pawn] | pieceBitboards[black][pawn];
        if (pieceType == knight)
            return pieceBitboards[white][knight] | pieceBitboards[black][knight];
        if (pieceType == king)
            return pieceBitboards[white][king] | pieceBitboards[black][king];
        if (pieceType == rook)
            return pieceBitboards[white][rook] | pieceBitboards[black][rook];
        if (pieceType == bishop)
            return pieceBitboards[white][bishop] | pieceBitboards[black][bishop];
        if (pieceType == queen)
            return pieceBitboards[white][queen] | pieceBitboards[black][queen];
    };

    void resetChessBoard();

private:
    ZobristHashGenerator zobristHasher = ZobristHashGenerator();

    void FENPiecesPlacement(std::string piecesPosition);
    void FENActiveColor(char aspect);
    void FENCastlingStates(std::string castlingPermision);
    void FENEnPassantAny(std::string enPassantString);
    void FENHalfMoveClock(std::string clock);
    void FENFullMoveNumber(std::string moveNumber);

    void updateCastlingRights(Color sideToMove, Color enemyColor, int from, int to);
    void updateFullmoveNumber(Color sideToMove);
    void updateHalfmoveClock(MoveFlags flag, PieceType movingPiece);
    void updateRepetitionTracker(U64 zobristKey, int changeAmount);
};

#endif