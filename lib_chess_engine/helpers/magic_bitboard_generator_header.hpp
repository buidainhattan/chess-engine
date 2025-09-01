#ifndef MAGIC_BITBOARD_GENERATOR_H
#define MAGIC_BITBOARD_GENERATOR_H

#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>

#include "bitboard_helper_header.hpp"

using attackMaskFunction = U64 (*)(int);
using allLegalAttacksFunction = std::vector<U64> (*)(int, std::vector<U64>);

struct MagicBitBoardAndShift
{
    U64 magicBitBoard;
    int shiftAmount;
};

class MagicBitBoardGenerator
{
    U64 seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937_64 randomGenerator{seed};

public:
    std::vector<U64> rookMagicBitBoards = std::vector<U64>(64);
    std::vector<int> rookShifts = std::vector<int>(64);
    std::vector<U64> bishopMagicBitBoards = std::vector<U64>(64);
    std::vector<int> bishopShifts = std::vector<int>(64);

    U64 generateRandomNumber();
    void printResult(std::vector<U64> &magicBitBoards, std::vector<int> &shiftAmounts);
    void findMagicNumberForAllSquares();
    MagicBitBoardAndShift findMagicNumberForGivenSquare(int square, attackMaskFunction attackMaskGen, allLegalAttacksFunction allLegalAttacksGen);
};

#endif