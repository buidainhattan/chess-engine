#include "magic_bitboard_generator_header.hpp"

using namespace std;

U64 MagicBitBoardGenerator::generateRandomNumber()
{
    return randomGenerator() & randomGenerator() & randomGenerator();
}

void MagicBitBoardGenerator::printResult(vector<U64> &magicBitBoards, vector<int> &shiftAmounts)
{
    cout << "{";
    for (U64 magicNumber : magicBitBoards)
    {
        cout << magicNumber << "ULL, ";
    }
    cout << "}" << endl;

    cout << "{";
    for (U64 shiftAmount : shiftAmounts)
    {
        cout << shiftAmount << ", ";
    }
    cout << "}" << endl;
}

void MagicBitBoardGenerator::findMagicNumberForAllSquares()
{
    cout << "Finding rook magic bitboard..." << endl;
    for (int square = 0; square < 64; square++)
    {
        cout << "Current square: " << square << endl;

        MagicBitBoardAndShift bitBoardAndShiftAmount = findMagicNumberForGivenSquare(square, rookRayMask, allRookAttackMasks);
        rookMagicBitBoards[square] = bitBoardAndShiftAmount.magicBitBoard;
        rookShifts[square] = bitBoardAndShiftAmount.shiftAmount;

        cout << endl;
    }

    cout << "Finding bishop magic bitboard..." << endl;
    for (int square = 0; square < 64; square++)
    {
        cout << "Current square: " << square << endl;

        MagicBitBoardAndShift bitBoardAndShiftAmount = findMagicNumberForGivenSquare(square, &bishopRayMask, allBishopAttackMasks);
        bishopMagicBitBoards[square] = bitBoardAndShiftAmount.magicBitBoard;
        bishopShifts[square] = bitBoardAndShiftAmount.shiftAmount;
    }
    
    cout << "Result of rook magic bitboard for each square and shift amount:" << endl;
    printResult(rookMagicBitBoards, rookShifts);
    cout << "Result of bishop magic bitboard for each square and shift amount:" << endl;
    printResult(bishopMagicBitBoards, bishopShifts);
}

MagicBitBoardAndShift MagicBitBoardGenerator::findMagicNumberForGivenSquare(int square, attackMaskFunction attackMaskGen, allLegalAttacksFunction allLegalAttacksGen)
{
    U64 attackMask = (attackMaskGen)(square);
    int shiftAmount = 64 - countBitBoard(attackMask);

    vector<U64> blockerBitBoards = allBlockerBitBoards(attackMask);
    vector<U64> allPseudoLegalAttacks = (allLegalAttacksGen)(square, blockerBitBoards);

    while (true)
    {
        U64 potentialMagicNumber = generateRandomNumber();

        bool collision = false;
        U64 usedIndex[4096] = {0ULL};

        for (int i = 0; i < blockerBitBoards.size(); i++)
        {
            U64 blockerBitBoard = blockerBitBoards[i];
            U64 index = (blockerBitBoard * potentialMagicNumber) >> shiftAmount;

            if (usedIndex[index] != 0)
            {
                break;
            }
            usedIndex[index] = allPseudoLegalAttacks[i];
        }

        if (!collision)
        {
            MagicBitBoardAndShift magicFound;
            magicFound.magicBitBoard = potentialMagicNumber;
            magicFound.shiftAmount = shiftAmount;
            return magicFound;
            break;
        }
    }
}
