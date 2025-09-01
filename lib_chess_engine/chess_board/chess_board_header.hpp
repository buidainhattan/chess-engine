#ifndef CHESS_BOARD_H
#define CHESS_BOARD_H

#include <sstream>
#include <cctype>
#include <stack>

#include "../helpers/bitboard_helper_header.hpp"

class ChessBoard
{
public:
    U64 pieceBitboards[color_NB][pieceType_NB] = {0ULL};
    U64 pieceColorBitboards[color_NB] = {0ULL};
    PieceType pieceAt[square_NB] = {pieceType_NB};

    U64 allPieces, empty;

    std::stack<BoardState> stateHistory = {};
    BoardState currentState;

    int halfMoveClock;
    int fullMoveNumber;

    void newGameSetup();
    void loadFromFEN(std::string FEN);
    void FENStringParser(std::string FEN);
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

    // Count material value of each side
    int countMaterials();

private:
    std::string startFEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

    void FENPiecesPlacement(std::string piecesPosition);
    void FENActiveColor(char aspect);
    void FENCastlingStates(std::string castlingPermision);
    void FENEnPassantAny(std::string enPassantString);
    void FENHalfMoveClock(std::string clock);
    void FENFullMoveNumber(std::string moveNumber);

    void updateCastlingRights(Color sideToMove, Color enemyColor, int from, int to);
};

#endif