#include "engine_header.hpp"

using namespace std;

Move EngineLite::convertStringToMove(const string &moveString)
{
    if (moveString.length() < 4)
    {
        throw std::runtime_error("Invalid UCI move string: too short");
    }

    // 1. Parse 'from' and 'to' squares
    string fromString = moveString.substr(0, 2);
    int from = stringToSquareEnum.find(fromString)->second;

    string toString = moveString.substr(2, 2);
    int to = stringToSquareEnum.find(toString)->second;

    PieceType moving_piece = chessBoard.pieceAt[from];
    MoveFlags flags = QUIET_MOVE;

    // 2. Handle Promotions
    if (moveString.length() == 5)
    {
        bool is_capture = chessBoard.pieceAt[to] != pieceType_NB;
        char promotion_char = moveString[4];
        switch (promotion_char)
        {
        case 'q':

            flags = is_capture ? PROMOTION_CAPTURE : PROMOTION;
            break;
        case 'r':
            flags = is_capture ? PROMOTION_CAPTURE : PROMOTION;
            break;
        case 'b':
            flags = is_capture ? PROMOTION_CAPTURE : PROMOTION;
            break;
        case 'n':
            flags = is_capture ? PROMOTION_CAPTURE : PROMOTION;
            break;
        default:
            throw std::runtime_error("Invalid promotion piece");
        }
        return Move(from, to, flags);
    }

    // 3. Handle Special Moves (using the board state)
    if (moving_piece == pawn)
    {
        // En Passant
        if (to == chessBoard.currentState.enPassantSquare)
        {
            flags = EN_PASSANT;
        }
        // Double Pawn Push
        else if (abs(from - to) / 8 == 2)
        {
            flags = DOUBLE_PAWN_PUSH;
        }
    }
    else if (moving_piece == king)
    {
        // Castling (king moves two squares)
        if (abs(from - to) / 8 == 2)
        {
            flags = (to > from) ? KING_CASTLE : QUEEN_CASTLE;
        }
    }

    // 4. Handle Captures (if not a special move already identified)
    if (flags == QUIET_MOVE && chessBoard.pieceAt[to] != pieceType_NB)
    {
        flags = CAPTURE;
    }

    return Move(from, to, flags);
}
