#ifndef BITBOARD_HELPER_H
#define BITBOARD_HELPER_H

#include <iostream>
#include <string>
#include <vector>

#include "../generals/all_enums_header.hpp"
#include "../generals/general_configs_header.hpp"

// Rook ray mask (emtpy board) and attack mask
std::vector<U64> allRookAttackMasks(int square, std::vector<U64> allBlockerBitBoards);
U64 rookAttackMask(int square, U64 occupancy);
U64 rookRayMask(int square);

// Bishop ray mask (empty board) and attack mask
std::vector<U64> allBishopAttackMasks(int square, std::vector<U64> allBlockerBitBoards);
U64 bishopAttackMask(int square, U64 occupancy);
U64 bishopRayMask(int square);

// Return all variations of blocker bitboard based on occupancy mask
std::vector<U64> allBlockerBitBoards(U64 occupancyMask);

// Basic bitboard operation
void setBit(U64 &bitBoard, int square);
void clearBit(U64 &bitBoard, int square);
void moveBit(U64 &bitBoard, int fromSquare, int toSquare);
void popLSB(U64 &bitboard);
void printBitBoard(U64 bitBoard);

// Bitboard shifting helper methods
U64 southShiftOne(U64 bitBoard);
U64 southShiftDouble(U64 bitBoard);
U64 eastShiftOne(U64 bitBoard);
U64 northShiftOne(U64 bitBoard);
U64 northShiftDouble(U64 bitBoard);
U64 westShiftOne(U64 bitBoard);
U64 southEastShiftOne(U64 bitBoard);
U64 southWestShiftOne(U64 bitBoard);
U64 northEastShiftOne(U64 bitBoard);
U64 northWestShiftOne(U64 bitBoard);

int countBitBoard(U64 bitBoard);
U64 getLSB(U64 bitBoard);
int bitScanForward(U64 bitboard);

#endif