#include "engine_header.hpp"

using namespace std;

Engine::Engine(ChessBoard &chessBoard) : chessBoard(chessBoard)
{
    generateNonSlidingPieceLookupTable(pawnAttacksLookupTable[white], pawn, white);
    generateNonSlidingPieceLookupTable(pawnAttacksLookupTable[black], pawn, black);
    generateNonSlidingPieceLookupTable(knightAttacksLookupTable, knight);
    generateNonSlidingPieceLookupTable(kingAttacksLookupTable, king);
    rookAttacksLookupTable = generateSlidingPieceLookupTable(rook);
    bishopAttacksLookupTable = generateSlidingPieceLookupTable(bishop);
    populateRectangular();
}

vector<Move> Engine::generateLegalMoves()
{
    vector<Move> legalMoves; // A vector to hold all valid moves
    legalMoves.reserve(218);
    pawnsFunctonPointer eastAttack, westAttack; // Pointers toward east, west attacks of pawn

    Color sideToMove = chessBoard.currentState.sideToMove; // Current legal side's turn
    Color enemyColor = Color(!sideToMove);                 // Enemy side

    U64 captureMask = ULLONG_MAX;
    U64 pushMask = ULLONG_MAX;
    int friendlyKingSquare = bitScanForward(chessBoard.pieceBitboards[sideToMove][king]);
    U64 checkers = attacksToKing(friendlyKingSquare, sideToMove);
    int numberOfChecks = countBitBoard(checkers); // Number of pieces currently checking the king
    bool isCheck = numberOfChecks == 1;
    bool isDoubleCheck = numberOfChecks == 2;

    // Current side friendly pieces, othogonal and diagonal pieces (include queen)
    U64 allPiecesNoFriendlyKing = chessBoard.allPieces & ~chessBoard.pieceBitboards[sideToMove][king];
    U64 friendlyPieces = chessBoard.pieceColorBitboards[sideToMove];
    U64 othogonalPieces = chessBoard.pieceBitboards[sideToMove][rook] | chessBoard.pieceBitboards[sideToMove][queen];
    U64 diagonalPieces = chessBoard.pieceBitboards[sideToMove][bishop] | chessBoard.pieceBitboards[sideToMove][queen];

    // Enemy side neccessary bitboard information
    U64 enemyPieces = chessBoard.pieceColorBitboards[enemyColor];
    U64 enemyPawns = chessBoard.pieceBitboards[enemyColor][pawn];

    // Set pointers above toward desired function based on enemy side
    if (enemyColor == white)
    {
        eastAttack = &Engine::whitePawnNEastAttacks;
        westAttack = &Engine::whitePawnNWestAttacks;
    }
    else
    {
        eastAttack = &Engine::blackPawnSWestAttacks;
        westAttack = &Engine::blackPawnSEastAttacks;
    }

    // Block of codes to find all dangered squares and pin ray of all enemy's sliding pieces
    // <---START--->
    // Bitboard contains squares that king is not allowed to move to
    U64 dangerSquaresMask = (this->*eastAttack)(enemyPawns) | (this->*westAttack)(enemyPawns);
    U64 pinRay = 0ULL;          // Bits that represent enemy's sliders attack ray
    U64 othogonalPinRay = 0ULL; // enemy othogonal sliders's attack ray
    U64 diagonalPinRay = 0ULL;  // enemy diagonal sliders's attack ray

    // Enemy king attacks mask
    int position = bitScanForward(chessBoard.pieceBitboards[enemyColor][king]);
    dangerSquaresMask |= kingAttacksLookupTable[position];

    U64 enemyKnights = chessBoard.pieceBitboards[enemyColor][knight];
    while (enemyKnights)
    {
        int position = bitScanForward(enemyKnights);
        popLSB(enemyKnights);
        dangerSquaresMask |= knightAttacksLookupTable[position];
    }

    U64 enemyRooks = chessBoard.pieceBitboards[enemyColor][rook] | chessBoard.pieceBitboards[enemyColor][queen];
    while (enemyRooks)
    {
        int position = bitScanForward(enemyRooks);
        U64 enemyRook = getLSB(enemyRooks);
        popLSB(enemyRooks);
        dangerSquaresMask |= getRookRawAttackBitBoard(position, allPiecesNoFriendlyKing);
        if (rookXRayAttacks(position, friendlyPieces, chessBoard.allPieces) & chessBoard.pieceBitboards[sideToMove][king])
        {
            othogonalPinRay |= (rectangularLookupTable[position][friendlyKingSquare] | enemyRook);
        }
    }

    U64 enemyBishops = chessBoard.pieceBitboards[enemyColor][bishop] | chessBoard.pieceBitboards[enemyColor][queen];
    while (enemyBishops)
    {
        int position = bitScanForward(enemyBishops);
        U64 enemyBishop = getLSB(enemyBishops);
        popLSB(enemyBishops);
        dangerSquaresMask |= getBishopRawAttackBitBoard(position, allPiecesNoFriendlyKing);
        if (bishopXRayAttacks(position, friendlyPieces, chessBoard.allPieces) & chessBoard.pieceBitboards[sideToMove][king])
        {
            diagonalPinRay |= (rectangularLookupTable[position][friendlyKingSquare] | enemyBishop);
        }
    }
    pinRay = othogonalPinRay | diagonalPinRay;
    // <---END--->

    // Skip other pieces move generation if is in double check
    // Only king's moves are valids
    // <---START--->
    generateKingMoves(legalMoves, sideToMove, chessBoard.pieceBitboards[sideToMove][king], enemyPieces, dangerSquaresMask);

    if (isDoubleCheck)
        return legalMoves;

    if (isCheck)
    {
        captureMask = checkers;

        int checkerSquare = bitScanForward(checkers);
        if (chessBoard.pieceAt[checkerSquare] >= 2 && chessBoard.pieceAt[checkerSquare] <= 4)
            pushMask = rectangularLookupTable[checkerSquare][friendlyKingSquare];
        else
            pushMask = 0ULL;
    }
    else
    {
        if (chessBoard.currentState.castlingRights)
        {
            U64 castlingRank = FIRST_RANK * enemyColor + EIGTH_RANK * sideToMove;
            if (chessBoard.currentState.canCastleQueenside(sideToMove))
            {
                if (!(chessBoard.allPieces & (squareBetweenQueenSide & castlingRank)))
                {
                    if (!((kingPathQueenSide & castlingRank) & dangerSquaresMask))
                        legalMoves.emplace_back(Move(friendlyKingSquare, c1 * enemyColor + c8 * sideToMove, QUEEN_CASTLE));
                }
            }

            if (chessBoard.currentState.canCastleKingside(sideToMove))
            {
                if (!(chessBoard.allPieces & (squareBetweenKingSide & castlingRank)))
                {
                    if (!((kingPathKingSide & castlingRank) & dangerSquaresMask))
                        legalMoves.emplace_back(Move(friendlyKingSquare, g1 * enemyColor + g8 * sideToMove, KING_CASTLE));
                }
            }
        }
    }

    generatePawnsMoves(legalMoves, sideToMove, chessBoard.pieceBitboards[sideToMove][pawn], friendlyKingSquare, enemyPieces, pushMask, captureMask, pinRay, othogonalPinRay, diagonalPinRay);
    generateKnightsMoves(legalMoves, sideToMove, chessBoard.pieceBitboards[sideToMove][knight], enemyPieces, pushMask, captureMask, pinRay);
    generateSlidingPiecesMoves(legalMoves, sideToMove, othogonalPieces, diagonalPieces, enemyPieces, pushMask, captureMask, pinRay, othogonalPinRay, diagonalPinRay);
    // <---END--->

    return legalMoves;
}

void Engine::generatePawnsMoves(vector<Move> &legalMoves, Color sideToMove, U64 pawns, int kingSquare, U64 enemyPieces, U64 pushMask, U64 captureMask, U64 pinRay, U64 othogonalPinRay, U64 diagonalPinRay)
{
    pawnsFunctonPointer singlePush, doublePush;
    pawnsFunctonPointer eastAttack, westAttack;
    int moveOffSet;
    U64 validEnpassantRank, validPromotionRank;

    if (sideToMove == white)
    {
        singlePush = &Engine::whitePawnSinglePush;
        doublePush = &Engine::whitePawnDoublePush;
        eastAttack = &Engine::whitePawnNEastAttacks;
        westAttack = &Engine::whitePawnNWestAttacks;
        moveOffSet = 1;
        validEnpassantRank = FIFTH_RANK;
        validPromotionRank = EIGTH_RANK;
    }
    else
    {
        singlePush = &Engine::blackPawnSinglePush;
        doublePush = &Engine::blackPawnDoublePush;
        eastAttack = &Engine::blackPawnSWestAttacks;
        westAttack = &Engine::blackPawnSEastAttacks;
        moveOffSet = -1;
        validEnpassantRank = FOURTH_RANK;
        validPromotionRank = FIRST_RANK;
    }

    // Seperate pinned and free pawns
    U64 pinnedPawns = pawns & pinRay;
    pawns &= ~pinRay;

    // Find any pawns that are eligble to perform enpassant
    int enPassantSquare = chessBoard.currentState.enPassantSquare;
    U64 enPassantAttackSquares = 0ULL;
    if (enPassantSquare != -1)
    {
        setBit(enPassantAttackSquares, enPassantSquare - 1);
        setBit(enPassantAttackSquares, enPassantSquare + 1);
        enPassantAttackSquares &= validEnpassantRank;
    }
    U64 enPassantPawns = pawns & enPassantAttackSquares;

    // pawns single and double push (if valid)
    U64 singlePushes, doublePushes = 0ULL;
    singlePushes = ((this->*singlePush)(pawns) | (this->*singlePush)(pinnedPawns)&othogonalPinRay) & pushMask;
    doublePushes = ((this->*doublePush)(pawns) | (this->*doublePush)(pinnedPawns)&othogonalPinRay) & pushMask;

    // pawns pseudo legal east and west attack
    U64 eastAttacks = ((this->*eastAttack)(pawns)&enemyPieces | (this->*eastAttack)(pinnedPawns)&enemyPieces & diagonalPinRay) & captureMask;
    U64 westAttacks = ((this->*westAttack)(pawns)&enemyPieces | (this->*westAttack)(pinnedPawns)&enemyPieces & diagonalPinRay) & captureMask;

    while (singlePushes)
    {
        int toSquare = bitScanForward(singlePushes);
        U64 singlePush = getLSB(singlePushes);
        popLSB(singlePushes);
        int fromSquare = toSquare - moveOffSet * 8;

        if (singlePush & validPromotionRank)
        {
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, queen, false));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, rook, false));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, bishop, false));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, knight, false));
            continue;
        }

        legalMoves.emplace_back(Move(fromSquare, toSquare, QUIET_MOVE));
    }

    while (doublePushes)
    {
        int toSquare = bitScanForward(doublePushes);
        popLSB(doublePushes);
        int fromSquare = toSquare - moveOffSet * 16;
        legalMoves.emplace_back(Move(fromSquare, toSquare, DOUBLE_PAWN_PUSH));
    }

    while (eastAttacks)
    {
        int toSquare = bitScanForward(eastAttacks);
        U64 attack = getLSB(eastAttacks);
        popLSB(eastAttacks);
        int fromSquare = toSquare - moveOffSet * 9;

        if (attack & validPromotionRank)
        {
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, queen, true));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, rook, true));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, bishop, true));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, knight, true));
            continue;
        }

        legalMoves.emplace_back(Move(fromSquare, toSquare, CAPTURE));
    }

    while (westAttacks)
    {
        int toSquare = bitScanForward(westAttacks);
        U64 attack = getLSB(westAttacks);
        popLSB(westAttacks);
        int fromSquare = toSquare - moveOffSet * 7;

        if (attack & validPromotionRank)
        {
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, queen, true));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, rook, true));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, bishop, true));
            legalMoves.emplace_back(Move().makePromotion(fromSquare, toSquare, knight, true));
            continue;
        }

        legalMoves.emplace_back(Move(fromSquare, toSquare, CAPTURE));
    }

    while (enPassantPawns)
    {
        int fromSquare = bitScanForward(enPassantPawns);
        popLSB(enPassantPawns);
        int toSquare = enPassantSquare + moveOffSet * 8;

        U64 allPiecesMask = chessBoard.allPieces;
        moveBit(allPiecesMask, fromSquare, toSquare);
        clearBit(allPiecesMask, enPassantSquare);
        if (!attacksToKing(kingSquare, sideToMove, allPiecesMask))
            legalMoves.emplace_back(Move(fromSquare, toSquare, EN_PASSANT));
    }
}

void Engine::generateKnightsMoves(vector<Move> &legalMoves, Color sideToMove, U64 knights, U64 enemyPieces, U64 pushMask, U64 captureMask, U64 pinRay)
{
    while (knights)
    {
        int fromSquare = bitScanForward(knights);
        U64 knight = getLSB(knights);
        popLSB(knights);
        if (pinRay & knight)
            continue;
        U64 attacks = getKnightPseudoLegalAttackBitBoard(fromSquare, sideToMove) & (pushMask | captureMask);

        while (attacks)
        {
            int toSquare = bitScanForward(attacks);
            U64 attack = getLSB(attacks);
            popLSB(attacks);
            MoveFlags flag = attack & enemyPieces ? CAPTURE : QUIET_MOVE;
            legalMoves.emplace_back(Move(fromSquare, toSquare, flag));
        }
    }
}

void Engine::generateKingMoves(vector<Move> &legalMoves, Color sideToMove, U64 king, U64 enemyPieces, U64 dangerSquaresMask)
{
    int fromSquare = bitScanForward(king);
    popLSB(king);
    U64 attacks = getKingPseudoLegalAttackBitBoard(fromSquare, sideToMove) & ~dangerSquaresMask;

    while (attacks)
    {
        int toSquare = bitScanForward(attacks);
        U64 attack = getLSB(attacks);
        popLSB(attacks);
        MoveFlags flag = attack & enemyPieces ? CAPTURE : QUIET_MOVE;
        legalMoves.emplace_back(Move(fromSquare, toSquare, flag));
    }
}

void Engine::generateSlidingPiecesMoves(vector<Move> &legalMoves, Color sideToMove, U64 othogonalPieces, U64 diagonalPieces, U64 enemyPieces, U64 pushMask, U64 captureMask, U64 pinRay, U64 othogonalPinRay, U64 diagonalPinRay)
{
    U64 othogonals = othogonalPieces;
    U64 diagonals = diagonalPieces;

    while (othogonals)
    {
        int fromSquare = bitScanForward(othogonals);
        U64 othogonal = getLSB(othogonals);
        popLSB(othogonals);
        U64 attacks = getRookPseudoLegalAttackBitBoard(fromSquare, chessBoard.allPieces, sideToMove) & (pushMask | captureMask);
        if (pinRay & othogonal)
        {
            if (othogonalPinRay & othogonal)
                attacks &= othogonalPinRay;
            else
                attacks = 0ULL;
        }

        while (attacks)
        {
            int toSquare = bitScanForward(attacks);
            U64 attack = getLSB(attacks);
            popLSB(attacks);
            MoveFlags flag = attack & enemyPieces ? CAPTURE : QUIET_MOVE;
            legalMoves.emplace_back(Move(fromSquare, toSquare, flag));
        }
    }

    while (diagonals)
    {
        int fromSquare = bitScanForward(diagonals);
        U64 diagonal = getLSB(diagonals);
        popLSB(diagonals);
        U64 attacks = getBishopPseudoLegalAttackBitBoard(fromSquare, chessBoard.allPieces, sideToMove) & (pushMask | captureMask);
        if (pinRay & diagonal)
        {
            if (diagonalPinRay & diagonal)
                attacks &= diagonalPinRay;
            else
                attacks = 0ULL;
        }

        while (attacks)
        {
            int toSquare = bitScanForward(attacks);
            U64 attack = getLSB(attacks);
            popLSB(attacks);
            MoveFlags flag = attack & enemyPieces ? CAPTURE : QUIET_MOVE;
            legalMoves.emplace_back(Move(fromSquare, toSquare, flag));
        }
    }
}

U64 Engine::whitePawnSinglePush(U64 whitePawns)
{
    return northShiftOne(whitePawns) & chessBoard.empty;
}

U64 Engine::whitePawnDoublePush(U64 whitePawns)
{
    U64 whitePawnSinglePushCheck = whitePawnSinglePush(whitePawns);
    return northShiftOne(whitePawnSinglePushCheck) & chessBoard.empty & FOURTH_RANK;
}

U64 Engine::blackPawnSinglePush(U64 blackPawns)
{
    return southShiftOne(blackPawns) & chessBoard.empty;
}

U64 Engine::blackPawnDoublePush(U64 blackPawns)
{
    U64 blackPawnSinglePushCheck = blackPawnSinglePush(blackPawns);
    return southShiftOne(blackPawnSinglePushCheck) & chessBoard.empty & FIFTH_RANK;
}

U64 Engine::whitePawnPseudoLegalCaptures(U64 whitePawns, U64 blackPieces)
{
    return blackPieces & whitePawnsAnyAttacks(whitePawns);
}

U64 Engine::blackPawnPseudoLegalCaptures(U64 blackPawns, U64 whitePieces)
{
    return whitePieces & blackPawnAnyAttacks(blackPawns);
}

U64 Engine::whitePawnSafeSquares(U64 whitePawns, U64 blackPawns)
{
    return whitePawnDuoAttacks(whitePawns) | ~blackPawnAnyAttacks(blackPawns) | (whitePawnSoloAttacks(whitePawns) & ~blackPawnDuoAttacks(blackPawns));
}

U64 Engine::blackPawnSafeSquares(U64 whitePawns, U64 blackPawns)
{
    return blackPawnDuoAttacks(blackPawns) | ~whitePawnsAnyAttacks(whitePawns) | (blackPawnSoloAttacks(blackPawns) & ~whitePawnDuoAttacks(whitePawns));
}

U64 Engine::getPawnPseudoLegalAttackBitBoard(int square, Color color)
{
    return (pawnAttacksLookupTable[color][square] & chessBoard.pieceColorBitboards[!color]);
}

U64 Engine::getKnightPseudoLegalAttackBitBoard(int square, Color color)
{
    return (knightAttacksLookupTable[square] & ~chessBoard.pieceColorBitboards[color]);
}

U64 Engine::getKingPseudoLegalAttackBitBoard(int square, Color color)
{
    return (kingAttacksLookupTable[square] & ~chessBoard.pieceColorBitboards[color]);
}

U64 Engine::getRookRawAttackBitBoard(int square, U64 allPieces)
{
    U64 rookBlockerBitBoard, magicIndex;
    rookBlockerBitBoard = rookRayMask(square) & allPieces;
    magicIndex = (rookBlockerBitBoard * rookMagicBitBoards[square]) >> rookShifts[square];
    U64 lookupIndex = (square * MAX_ROOK_SIZE) + magicIndex;

    return rookAttacksLookupTable[lookupIndex];
}

U64 Engine::getRookPseudoLegalAttackBitBoard(int square, U64 allPieces, Color color)
{
    return getRookRawAttackBitBoard(square, allPieces) & ~chessBoard.pieceColorBitboards[color];
}

U64 Engine::getBishopRawAttackBitBoard(int square, U64 allPieces)
{
    U64 bishopBlockerBitBoard, magicIndex;
    bishopBlockerBitBoard = bishopRayMask(square) & allPieces;
    magicIndex = (bishopBlockerBitBoard * bishopMagicBitBoards[square]) >> bishopShifts[square];
    U64 lookupIndex = (square * MAX_BISHOP_SIZE) + magicIndex;

    return bishopAttacksLookupTable[lookupIndex];
}

U64 Engine::getBishopPseudoLegalAttackBitBoard(int square, U64 allPieces, Color color)
{
    return getBishopRawAttackBitBoard(square, allPieces) & ~chessBoard.pieceColorBitboards[color];
}

U64 Engine::squareAttackingBy(int square)
{
    U64 squareAttackingBy = ((pawnAttacksLookupTable[black][square] & chessBoard.pieceBitboards[white][pawn]) | (pawnAttacksLookupTable[white][square] & chessBoard.pieceBitboards[black][pawn]) | (rookAttackMask(square, chessBoard.allPieces) & (chessBoard.getAllPiecesOfType<rook>() | chessBoard.getAllPiecesOfType<queen>())) | (knightAttacksLookupTable[square] & chessBoard.getAllPiecesOfType<knight>()) | (bishopAttackMask(square, chessBoard.allPieces) & (chessBoard.getAllPiecesOfType<bishop>() | chessBoard.getAllPiecesOfType<queen>())) | (kingAttacksLookupTable[square] & chessBoard.getAllPiecesOfType<king>()));

    return squareAttackingBy;
}

U64 Engine::squareAttackingBy(int square, U64 allPieces)
{
    U64 squareAttackingBy = ((rookAttackMask(square, allPieces) & (chessBoard.getAllPiecesOfType<rook>() | chessBoard.getAllPiecesOfType<queen>())) | (bishopAttackMask(square, allPieces) & (chessBoard.getAllPiecesOfType<bishop>() | chessBoard.getAllPiecesOfType<queen>())));

    return squareAttackingBy;
}

U64 Engine::attacksToKing(int square, Color kingSide)
{
    U64 attacksToKing = (squareAttackingBy(square) & chessBoard.pieceColorBitboards[!kingSide]);

    return attacksToKing;
}

U64 Engine::attacksToKing(int square, Color kingSide, U64 allPieces)
{
    U64 attacksToKing = squareAttackingBy(square, allPieces) & chessBoard.pieceColorBitboards[!kingSide];

    return attacksToKing;
}

U64 Engine::absolutePinPieces(int square, Color kingColor)
{
    U64 enemyRooks = kingColor == white ? chessBoard.pieceBitboards[black][rook] : chessBoard.pieceBitboards[white][rook];
    U64 enemyBishops = kingColor == white ? chessBoard.pieceBitboards[black][bishop] : chessBoard.pieceBitboards[white][bishop];

    U64 pinned = 0;
    U64 pinner = rookXRayAttacks(square, chessBoard.pieceColorBitboards[kingColor], chessBoard.allPieces) & enemyRooks;
    while (pinner)
    {
        int sq = bitScanForward(pinner);
        pinned |= rectangularLookupTable[sq][square] & chessBoard.pieceColorBitboards[kingColor];
        pinner &= pinner - 1;
    }
    pinner = bishopXRayAttacks(square, chessBoard.pieceColorBitboards[kingColor], chessBoard.allPieces) & enemyBishops;
    while (pinner)
    {
        int sq = bitScanForward(pinner);
        pinned |= rectangularLookupTable[sq][square] & chessBoard.pieceColorBitboards[kingColor];
        pinner &= pinner - 1;
    }

    return pinned;
}

bool Engine::canMove(int fromSquare, int toSquare, U64 blockers)
{
    return ((rectangularLookupTable[fromSquare][toSquare] & blockers) == 0);
}

U64 Engine::rookXRayAttacks(int square, U64 blockers, U64 occupancy)
{
    U64 attacks = rookAttackMask(square, occupancy);
    blockers &= attacks;

    return attacks ^ rookAttackMask(square, occupancy ^ blockers);
}

U64 Engine::bishopXRayAttacks(int square, U64 blockers, U64 occupancy)
{
    U64 attacks = bishopAttackMask(square, occupancy);
    blockers &= attacks;

    return attacks ^ bishopAttackMask(square, occupancy ^ blockers);
}

U64 Engine::generateKnightAttacksMask(int square)
{
    U64 knightAttacks = 0ULL;
    U64 knightPosition = 0ULL;
    setBit(knightPosition, square);

    // Left side knight's attacks
    knightAttacks |= (knightPosition << 15 | knightPosition >> 17) & ~H_FILE | (knightPosition << 6 | knightPosition >> 10) & ~GH_FILE;

    // Right side knight's attacks
    knightAttacks |= (knightPosition << 17 | knightPosition >> 15) & ~A_FILE | (knightPosition << 10 | knightPosition >> 6) & ~AB_FILE;

    return knightAttacks;
}

U64 Engine::generateKingAttacksMask(int square)
{
    U64 kingAttacks = 0ULL;
    U64 kingPosition = 0ULL;
    setBit(kingPosition, square);

    // Up and down king's attacks
    kingAttacks |= northShiftOne(kingPosition) | southShiftOne(kingPosition);

    // Left side king's attacks
    kingAttacks |= (kingPosition << 7 | kingPosition >> 1 | kingPosition >> 9) & ~H_FILE;

    // Right side king's attacks
    kingAttacks |= (kingPosition << 9 | kingPosition << 1 | kingPosition >> 7) & ~A_FILE;

    return kingAttacks;
}

U64 Engine::generateWhitePawnAttackMask(int square)
{
    U64 whitePawnAttacks = 0ULL;
    U64 whitePawnPosition = 0ULL;
    setBit(whitePawnPosition, square);

    whitePawnAttacks |= (northEastShiftOne(whitePawnPosition) | northWestShiftOne(whitePawnPosition));

    return whitePawnAttacks;
}

U64 Engine::generateBlackPawnAttackMask(int square)
{
    U64 blackPawnAttacks = 0ULL;
    U64 blackPawnPosition = 0ULL;
    setBit(blackPawnPosition, square);

    blackPawnAttacks |= (southEastShiftOne(blackPawnPosition) | southWestShiftOne(blackPawnPosition));

    return blackPawnAttacks;
}

U64 Engine::whitePawnSoloAttacks(U64 whitePawns)
{
    return whitePawnNEastAttacks(whitePawns) ^ whitePawnNWestAttacks(whitePawns);
}

U64 Engine::whitePawnDuoAttacks(U64 whitePawns)
{
    return whitePawnNEastAttacks(whitePawns) & whitePawnNWestAttacks(whitePawns);
}

U64 Engine::whitePawnsAnyAttacks(U64 whitePawns)
{
    return whitePawnNEastAttacks(whitePawns) | whitePawnNWestAttacks(whitePawns);
}

U64 Engine::whitePawnNEastAttacks(U64 whitePawns)
{
    return northEastShiftOne(whitePawns);
}

U64 Engine::whitePawnNWestAttacks(U64 whitePawns)
{
    return northWestShiftOne(whitePawns);
}

U64 Engine::blackPawnSoloAttacks(U64 blackPawns)
{
    return blackPawnSEastAttacks(blackPawns) ^ blackPawnSWestAttacks(blackPawns);
}

U64 Engine::blackPawnDuoAttacks(U64 blackPawns)
{
    return blackPawnSEastAttacks(blackPawns) & blackPawnSWestAttacks(blackPawns);
}

U64 Engine::blackPawnAnyAttacks(U64 blackPawns)
{
    return blackPawnSEastAttacks(blackPawns) | blackPawnSWestAttacks(blackPawns);
}

U64 Engine::blackPawnSEastAttacks(U64 blackPawns)
{
    return southEastShiftOne(blackPawns);
}

U64 Engine::blackPawnSWestAttacks(U64 blackPawns)
{
    return southWestShiftOne(blackPawns);
}

void Engine::generateNonSlidingPieceLookupTable(U64 (&arr)[64], PieceType pieceType, Color color)
{
    generateFunctionPointer moveGeneration;
    if (pieceType == pawn)
    {
        moveGeneration = color == white ? &Engine::generateWhitePawnAttackMask : &Engine::generateBlackPawnAttackMask;
    }
    else
    {
        moveGeneration = pieceType == knight ? &Engine::generateKnightAttacksMask : &Engine::generateKingAttacksMask;
    }

    for (int square = 0; square < 64; square++)
    {
        U64 attackMask = (this->*moveGeneration)(square);
        arr[square] = attackMask;
    }
}

vector<U64> Engine::generateSlidingPieceLookupTable(PieceType pieceType)
{
    int maxSquareSize = pieceType == rook ? MAX_ROOK_SIZE : MAX_BISHOP_SIZE;
    int ATTACK_TABLE_SIZE = 64 * maxSquareSize;
    vector<U64> lookupTable(ATTACK_TABLE_SIZE);

    for (int square = 0; square < 64; square++)
    {
        U64 occupancyMask, magicBitBoard;
        vector<U64> blockerBitBoards, allLegalAttackBitBoards;
        int shiftAmount;

        switch (pieceType)
        {
        case rook:
            occupancyMask = rookRayMask(square);
            magicBitBoard = rookMagicBitBoards[square];
            shiftAmount = rookShifts[square];
            break;

        default:
            occupancyMask = bishopRayMask(square);
            magicBitBoard = bishopMagicBitBoards[square];
            shiftAmount = bishopShifts[square];
            break;
        }

        blockerBitBoards = allBlockerBitBoards(occupancyMask);

        allLegalAttackBitBoards = pieceType == rook ? allRookAttackMasks(square, blockerBitBoards) : allBishopAttackMasks(square, blockerBitBoards);

        for (int i = 0; i < blockerBitBoards.size(); i++)
        {
            U64 index = (blockerBitBoards[i] * magicBitBoard) >> shiftAmount;
            lookupTable[square * maxSquareSize + index] = allLegalAttackBitBoards[i];
        }
    }

    return lookupTable;
}

void Engine::populateRectangular()
{
    for (int square1 = 0; square1 < 64; square1++)
    {
        for (int square2 = 0; square2 < 64; square2++)
        {
            if (square1 == square2)
            {
                rectangularLookupTable[square1][square2] = 0ULL;
                continue;
            }

            U64 inBetweenBitBoard, square1BitBoard, square2BitBoard;
            inBetweenBitBoard = square1BitBoard = square2BitBoard = 0ULL;
            setBit(square1BitBoard, square1);
            setBit(square2BitBoard, square2);
            U64 startBitBoard, endBitBoard;
            if (square1 < square2)
            {
                startBitBoard = square1BitBoard;
                endBitBoard = square2BitBoard;
            }
            else
            {
                startBitBoard = square2BitBoard;
                endBitBoard = square1BitBoard;
            }

            bool sameRank = ((square1 / 8) == (square2 / 8));
            if (sameRank)
            {
                while ((eastShiftOne(startBitBoard) & endBitBoard) == 0)
                {
                    inBetweenBitBoard |= eastShiftOne(startBitBoard);
                    startBitBoard = eastShiftOne(startBitBoard);
                }
            }

            bool sameFile = ((square1 % 8) == (square2 % 8));
            if (sameFile)
            {
                while ((northShiftOne(startBitBoard) & endBitBoard) == 0)
                {
                    inBetweenBitBoard |= northShiftOne(startBitBoard);
                    startBitBoard = northShiftOne(startBitBoard);
                }
            }

            bool samePositiveDiagonal = ((square1 >> 3) - (square1 & 7) == (square2 >> 3) - (square2 & 7));
            if (samePositiveDiagonal)
            {
                while ((northEastShiftOne(startBitBoard) & endBitBoard) == 0)
                {
                    inBetweenBitBoard |= northEastShiftOne(startBitBoard);
                    startBitBoard = northEastShiftOne(startBitBoard);
                }
            }

            bool sameNegativeDiagonal = ((square1 >> 3) + (square1 & 7) == (square2 >> 3) + (square2 & 7));
            if (sameNegativeDiagonal)
            {
                while ((northWestShiftOne(startBitBoard) & endBitBoard) == 0)
                {
                    inBetweenBitBoard |= northWestShiftOne(startBitBoard);
                    startBitBoard = northWestShiftOne(startBitBoard);
                }
            }

            rectangularLookupTable[square1][square2] = inBetweenBitBoard;
        }
    }
}
