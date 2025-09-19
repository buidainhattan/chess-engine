#ifndef ALL_ENUMS_H
#define ALL_ENUMS_H

#include <string>
#include <map>

// Little-Endian Rank-File mapping
const enum SquareEnum {
    a1, b1, c1, d1, e1, f1, g1, h1,
    a2, b2, c2, d2, e2, f2, g2, h2,
    a3, b3, c3, d3, e3, f3, g3, h3,
    a4, b4, c4, d4, e4, f4, g4, h4,
    a5, b5, c5, d5, e5, f5, g5, h5,
    a6, b6, c6, d6, e6, f6, g6, h6,
    a7, b7, c7, d7, e7, f7, g7, h7,
    a8, b8, c8, d8, e8, f8, g8, h8,
    square_NB,
};

extern const std::map<std::string, SquareEnum> stringToSquareEnum;
extern const std::string squareIndexToString[64];
extern const std::string pieceTypeToString[7];
extern const std::string colorToString[3];

const enum PieceType {
    pawn, knight, bishop, rook, queen, king,
    pieceType_NB,
};

static const int pieceValue[6] = {100, 320, 330, 500, 900, 20000};

const enum Color {
    white, black,
    color_NB,
};

// Flag constants
const enum MoveFlags {
    QUIET_MOVE = 0,
    DOUBLE_PAWN_PUSH = 1,
    KING_CASTLE = 2,
    QUEEN_CASTLE = 3,
    CAPTURE = 4,
    EN_PASSANT = 5,
    PROMOTION = 8,
    PROMOTION_CAPTURE = 12,
};

struct BoardState
{
    PieceType captured = pieceType_NB;
    int enPassantSquare = -1; // -1 if no en passant possible
    int castlingRights = 0;   // KQkq bits: 1111 = all castling available
    int halfmoveClock = 0;    // 50-move rule counter
    Color sideToMove = white; // 0 = WHITE, 1 = BLACK
    bool isInCheck = false;

    BoardState() = default;

    BoardState(int ep, int castle, int half, Color side)
        : enPassantSquare(ep), castlingRights(castle), halfmoveClock(half), sideToMove(side) {}

    // Castling rights helpers
    void addCastlingRights(Color color, bool isKingSide)
    {
        int bit = color * 2 + (isKingSide ? 0 : 1);
        castlingRights |= (1 << bit);
    }

    void addAllCastlingRights(Color color)
    {
        int mask = 3 << (color * 2); // Add both bits for the color
        castlingRights |= ~mask;
    }

    bool canCastleKingside(int color) const
    {
        return castlingRights & (1 << (color * 2)); // WHITE: bit 1, BLACK: bit 3
    }

    bool canCastleQueenside(int color) const
    {
        return castlingRights & (1 << (color * 2 + 1)); // WHITE: bit 2, BLACK: bit 4
    }

    void removeCastlingRights(Color color, bool isKingSide)
    {
        int bit = color * 2 + (isKingSide ? 0 : 1);
        castlingRights &= ~(1 << bit);
    }

    void removeAllCastlingRights(Color color)
    {
        int mask = 3 << (color * 2); // Remove both bits for the color
        castlingRights &= ~mask;
    }
};

struct Move
{
    uint16_t data;

    // Stockfish bit layout: from(6) | to(6) | flags(4)

    Move() : data(0) {}
    Move(int from, int to, MoveFlags flags = QUIET_MOVE)
    {
        data = from | (to << 6) | (flags << 12);
    }

    int from() const { return data & 0x3F; }
    int to() const { return (data >> 6) & 0x3F; }
    MoveFlags flags() const { return MoveFlags((data >> 12) & 0xF); }

    static Move makePromotion(int from, int to, PieceType piece, bool capture = false)
    {
        int flags = 8 | (piece - knight); // bit 3 = promotion, bits 0-1 = piece
        if (capture)
            flags |= 4; // bit 2 = capture
        return Move(from, to, MoveFlags(flags));
    }

    PieceType promotionPiece() const
    {
        return (flags() & 8) ? PieceType(knight + (flags() & 3)) : pieceType_NB;
    }

    bool isPromotion() const { return flags() & 8; }
    bool isCapture() const { return flags() & 4; }

    bool operator==(const Move &other) const { return data == other.data; }
    bool operator!=(const Move &other) const { return data != other.data; }
};

static const int pieceSquareTable[pieceType_NB][square_NB] = 
{
    {
         0,  0,  0,  0,  0,  0,  0,  0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
         5,  5, 10, 25, 25, 10,  5,  5,
         0,  0,  0, 20, 20,  0,  0,  0,
         5, -5,-10,  0,  0,-10, -5,  5,
         5, 10, 10,-20,-20, 10, 10,  5,
         0,  0,  0,  0,  0,  0,  0,  0,
    },
    {
        -50,-40,-30,-30,-30,-30,-40,-50,
        -40,-20,  0,  0,  0,  0,-20,-40,
        -30,  0, 10, 15, 15, 10,  0,-30,
        -30,  5, 15, 20, 20, 15,  5,-30,
        -30,  0, 15, 20, 20, 15,  0,-30,
        -30,  5, 10, 15, 15, 10,  5,-30,
        -40,-20,  0,  5,  5,  0,-20,-40,
        -50,-40,-30,-30,-30,-30,-40,-50,
    },
    {
        -20,-10,-10,-10,-10,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5, 10, 10,  5,  0,-10,
        -10,  5,  5, 10, 10,  5,  5,-10,
        -10,  0, 10, 10, 10, 10,  0,-10,
        -10, 10, 10, 10, 10, 10, 10,-10,
        -10,  5,  0,  0,  0,  0,  5,-10,
        -20,-10,-10,-10,-10,-10,-10,-20, 
    },
    {
        0,  0,  0,  0,  0,  0,  0,  0,
        5, 10, 10, 10, 10, 10, 10,  5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
       -5,  0,  0,  0,  0,  0,  0, -5,
        0,  0,  0,  5,  5,  0,  0,  0,
    },
    {
        -20,-10,-10, -5, -5,-10,-10,-20,
        -10,  0,  0,  0,  0,  0,  0,-10,
        -10,  0,  5,  5,  5,  5,  0,-10,
         -5,  0,  5,  5,  5,  5,  0, -5,
          0,  0,  5,  5,  5,  5,  0, -5,
        -10,  5,  5,  5,  5,  5,  0,-10,
        -10,  0,  5,  0,  0,  0,  0,-10,
        -20,-10,-10, -5, -5,-10,-10,-20,
    },
    {
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -30,-40,-40,-50,-50,-40,-40,-30,
        -20,-30,-30,-40,-40,-30,-30,-20,
        -10,-20,-20,-20,-20,-20,-20,-10,
         20, 20,  0,  0,  0,  0, 20, 20,
         20, 30, 10,  0,  0, 10, 30, 20,
    },
};

#endif