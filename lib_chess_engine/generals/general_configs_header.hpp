#ifndef GENERAL_CONFIGS_H
#define GENERAL_CONFIGS_H

#define _Compiletime __forceinline static constexpr

using U64 = unsigned long long;

static constexpr U64 A_FILE = 0x0101010101010101ULL;
static constexpr U64 H_FILE = 0x8080808080808080ULL;
static constexpr U64 AB_FILE = 0x0303030303030303ULL;
static constexpr U64 GH_FILE = 0xC0C0C0C0C0C0C0C0ULL;

static constexpr U64 FIRST_RANK = 0x00000000000000FFULL;
static constexpr U64 FOURTH_RANK = 0x00000000FF000000ULL;
static constexpr U64 FIFTH_RANK = 0x000000FF00000000ULL;
static constexpr U64 EIGTH_RANK = 0xFF00000000000000ULL;

static constexpr U64 northEastEdge = EIGTH_RANK | H_FILE;
static constexpr U64 southEastEdge = FIRST_RANK | H_FILE;
static constexpr U64 northWestEdge = EIGTH_RANK | A_FILE;
static constexpr U64 southWestEdge = FIRST_RANK | A_FILE;

static constexpr U64 squareBetweenQueenSide = 1008806316530991118ULL;
static constexpr U64 squareBetweenKingSide = 6917529027641081952ULL;
static constexpr U64 castlingQueenSideTargetSquare = 288230376151711748ULL;
static constexpr U64 castlingKingSideTargetSquare = 4611686018427387968ULL;
static constexpr U64 kingPathQueenSide = 864691128455135244ULL;
static constexpr U64 kingPathKingSide = 6917529027641081952ULL;

static constexpr int MAX_ROOK_SIZE = 4096;
static constexpr int MAX_BISHOP_SIZE = 512;

static constexpr int NEGATIVE_INFINITY = -100000;
static constexpr int POSITIVE_INFINITY = 100000;

#endif