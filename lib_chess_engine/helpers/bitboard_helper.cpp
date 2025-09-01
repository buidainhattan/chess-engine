#include <bit>

#include "bitboard_helper_header.hpp"

using namespace std;

vector<U64> allRookAttackMasks(int square, vector<U64> allBlockerBitBoards)
{
    vector<U64> allRookAttackMasks = {};

    for (U64 blockerBitBoard : allBlockerBitBoards)
    {
        U64 pseudoLegalAttack = rookAttackMask(square, blockerBitBoard);
        allRookAttackMasks.push_back(pseudoLegalAttack);
    }

    return allRookAttackMasks;
}

U64 rookAttackMask(int square, U64 occupancy)
{
    U64 rookAttackMask = 0ULL;
    U64 rookPosition = 0ULL;
    setBit(rookPosition, square);
    U64 northAttack, eastAttack, southAttack, westAttack;
    northAttack = eastAttack = southAttack = westAttack = rookPosition;
    bool northBlocked, eastBlocked, southBlocked, westBlocked;
    northBlocked = eastBlocked = southBlocked = westBlocked = false;

    do
    {
        northAttack = northShiftOne(northAttack);
        rookAttackMask |= northAttack;
        if ((northAttack & occupancy) != 0 || (northAttack & EIGTH_RANK) != 0 || (rookPosition & EIGTH_RANK) != 0)
        {
            northBlocked = true;
        }
    } while (!northBlocked);
    do
    {
        eastAttack = eastShiftOne(eastAttack);
        rookAttackMask |= eastAttack;
        if ((eastAttack & occupancy) != 0 || (eastAttack & H_FILE) != 0 || (rookPosition & H_FILE) != 0)
        {
            eastBlocked = true;
        }
    } while (!eastBlocked);
    do
    {
        southAttack = southShiftOne(southAttack);
        rookAttackMask |= southAttack;
        if ((southAttack & occupancy) != 0 || (southAttack & FIRST_RANK) != 0 || (rookPosition & FIRST_RANK) != 0)
        {
            southBlocked = true;
        }
    } while (!southBlocked);
    do
    {
        westAttack = westShiftOne(westAttack);
        rookAttackMask |= westAttack;
        if ((westAttack & occupancy) != 0 || (westAttack & A_FILE) != 0 || (rookPosition & A_FILE) != 0)
        {
            westBlocked = true;
        }
    } while (!westBlocked);

    return rookAttackMask;
}

U64 rookRayMask(int square)
{
    U64 rookAttackMask = 0ULL;
    U64 rookPosition = 0ULL;
    setBit(rookPosition, square);
    U64 northAttack, eastAttack, southAttack, westAttack;
    northAttack = eastAttack = southAttack = westAttack = rookPosition;

    for (int i = 0; i < 6; i++)
    {
        northAttack = northShiftOne(northAttack) & ~EIGTH_RANK;
        eastAttack = eastShiftOne(eastAttack) & ~H_FILE;
        southAttack = southShiftOne(southAttack) & ~FIRST_RANK;
        westAttack = westShiftOne(westAttack) & ~A_FILE;

        rookAttackMask |= northAttack | eastAttack | southAttack | westAttack;
    }

    return rookAttackMask;
}

vector<U64> allBishopAttackMasks(int square, std::vector<U64> allBlockerBitBoards)
{
    vector<U64> allBishopAttackMasks;

    for (U64 blockerBitBoard : allBlockerBitBoards)
    {
        U64 pseudoLegalAttack = bishopAttackMask(square, blockerBitBoard);
        allBishopAttackMasks.push_back(pseudoLegalAttack);
    }

    return allBishopAttackMasks;
}

U64 bishopAttackMask(int square, U64 occupancy)
{
    U64 bishopAttackMask = 0ULL;
    U64 bishopPosition = 0Ull;
    setBit(bishopPosition, square);
        U64 northEastAttack, southEastAttack, northWestAttack, southWestAttack;
    northEastAttack = southEastAttack = northWestAttack = southWestAttack = bishopPosition;
    do
    {
        northEastAttack = northEastShiftOne(northEastAttack);
        bishopAttackMask |= northEastAttack;
        if ((northEastAttack & occupancy) != 0 || (northEastAttack & northEastEdge) != 0)
        {
            break;
        }
    } while ((bishopPosition & northEastEdge) == 0);
    do
    {
        southEastAttack = southEastShiftOne(southEastAttack);
        bishopAttackMask |= southEastAttack;
        if ((southEastAttack & occupancy) != 0 || (southEastAttack & southEastEdge) != 0)
        {
            break;
        }
    } while ((bishopPosition & southEastEdge) == 0);
    do
    {
        northWestAttack = northWestShiftOne(northWestAttack);
        bishopAttackMask |= northWestAttack;
        if ((northWestAttack & occupancy) != 0 || (northWestAttack & northWestEdge) != 0)
        {
            break;
        }
    } while ((bishopPosition & northWestEdge) == 0);
    do
    {
        southWestAttack = southWestShiftOne(southWestAttack);
        bishopAttackMask |= southWestAttack;
        if ((southWestAttack & occupancy) != 0 || (southWestAttack & southWestEdge) != 0)
        {
            break;
        }
    } while ((bishopPosition & southWestEdge) == 0);

    return bishopAttackMask;
}

U64 bishopRayMask(int square)
{
    U64 bishopAttackMask = 0ULL;
    U64 bishopPosition = 0ULL;
    setBit(bishopPosition, square);
    U64 northEastAttack, southEastAttack, northWestAttack, southWestAttack;
    northEastAttack = southEastAttack = northWestAttack = southWestAttack = bishopPosition;
    U64 northEastEdge = ~(EIGTH_RANK | H_FILE);
    U64 southEastEdge = ~(FIRST_RANK | H_FILE);
    U64 northWestEdge = ~(EIGTH_RANK | A_FILE);
    U64 southWestEdge = ~(FIRST_RANK | A_FILE);

    for (int i = 0; i < 6; i++)
    {
        northEastAttack = northEastShiftOne(northEastAttack) & (northEastEdge);
        southEastAttack = southEastShiftOne(southEastAttack) & (southEastEdge);
        northWestAttack = northWestShiftOne(northWestAttack) & (northWestEdge);
        southWestAttack = southWestShiftOne(southWestAttack) & (southWestEdge);

        bishopAttackMask |= northEastAttack | southEastAttack | northWestAttack | southWestAttack;
    }

    return bishopAttackMask;
}

vector<U64> allBlockerBitBoards(U64 attackMask)
{
    vector<int> attackSquareIndices = {};
    for (int i = 0; i < 64; i++)
    {
        if (((attackMask >> i) & 1) == 1)
        {
            attackSquareIndices.push_back(i);
        }
    }

    int numberOfPatterns = 1 << attackSquareIndices.size();
    vector<U64> blockerBitBoards(numberOfPatterns);

    for (int patternIndex = 0; patternIndex < numberOfPatterns; patternIndex++)
    {
        for (int bitIndex = 0; bitIndex < attackSquareIndices.size(); bitIndex++)
        {
            U64 bit = (patternIndex >> bitIndex) & 1;
            blockerBitBoards[patternIndex] |= bit << attackSquareIndices[bitIndex];
        }
    }

    return blockerBitBoards;
}

void setBit(U64 &bitBoard, int square)
{
    bitBoard |= (1ULL << square);
}

void clearBit(U64 &bitBoard, int square)
{
    bitBoard &= ~(1ULL << square);
}

void moveBit(U64 &bitBoard, int fromSquare, int toSquare)
{
    bitBoard ^= (1ULL << fromSquare) | (1ULL << toSquare);
}

void popLSB(U64 &bitboard)
{
    bitboard &= (bitboard - 1);
}

void printBitBoard(U64 bitBoard)
{
    for (int rank = 8; rank > 0; --rank)
    {
        for (int file = 8; file > 0; --file)
        {
            int square = (rank * 8) - file;
            U64 number = bitBoard >> square;
            U64 bit = number & 1;

            if (bit == 1)
            {
                cout << 1;
            }
            else
            {
                cout << 0;
            }
            cout << " ";
        }
        cout << "   " << rank << endl;
    }

    cout << endl
         << "a b c d e f g h" << endl;
    cout << "Bitboard number: " << bitBoard << endl;
}

U64 southShiftOne(U64 bitBoard)
{
    return (bitBoard >> 8);
}

U64 southShiftDouble(U64 bitBoard)
{
    return (bitBoard >> 16);
}

U64 eastShiftOne(U64 bitBoard)
{
    return ((bitBoard << 1) & ~A_FILE);
}

U64 northShiftOne(U64 bitBoard)
{
    return (bitBoard << 8);
}

U64 northShiftDouble(U64 bitBoard)
{
    return (bitBoard << 16);
}

U64 westShiftOne(U64 bitBoard)
{
    return ((bitBoard >> 1) & ~H_FILE);
}

U64 southEastShiftOne(U64 bitBoard)
{
    return ((bitBoard >> 7) & ~A_FILE);
}

U64 southWestShiftOne(U64 bitBoard)
{
    return ((bitBoard >> 9) & ~H_FILE);
}

U64 northEastShiftOne(U64 bitBoard)
{
    return ((bitBoard << 9) & ~A_FILE);
}

U64 northWestShiftOne(U64 bitBoard)
{
    return ((bitBoard << 7) & ~H_FILE);
}

int countBitBoard(U64 bitBoard)
{
    return popcount(bitBoard);
}

U64 getLSB(U64 bitBoard)
{
    U64 leastSB = (bitBoard & -bitBoard);
    return leastSB;
}

int bitScanForward(U64 bitboard)
{  
    return countr_zero(bitboard);
}

