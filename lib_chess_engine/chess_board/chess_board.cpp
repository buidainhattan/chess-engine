#include "chess_board_header.hpp"

using namespace std;

ChessBoard::ChessBoard()
{
    fill(pieceAt, pieceAt + square_NB, pieceType_NB);
}

void ChessBoard::loadFromFEN(string FEN)
{
    FENStringParser(FEN);
}

void ChessBoard::FENStringParser(string FEN)
{
    stringstream ss(FEN);
    string aspect;
    int aspectIndex = 0;
    while (getline(ss, aspect, ' '))
    {
        switch (aspectIndex)
        {
        case 0: // Piece placement
            FENPiecesPlacement(aspect);
            initialOccupancy();
            break;
        case 1: // Active color
            FENActiveColor(aspect[0]);
            break;
        case 2: // Castling avaiability
            FENCastlingStates(aspect);
            break;
        case 3: // Enpassant square
            FENEnPassantAny(aspect);
            break;
        case 4: // Half move clock
            FENHalfMoveClock(aspect);
            break;
        case 5: // Number of full moves
            FENFullMoveNumber(aspect);
            break;

        default: // Out of range
            break;
        }
        aspectIndex++;
    }
}

void ChessBoard::initialOccupancy()
{
    for (int pieceType = 0; pieceType < pieceType_NB; pieceType++)
    {
        pieceColorBitboards[white] |= pieceBitboards[white][pieceType];
        pieceColorBitboards[black] |= pieceBitboards[black][pieceType];
    }
    allPieces = (pieceColorBitboards[white] | pieceColorBitboards[black]);
    empty = ~allPieces;
}

void ChessBoard::switchActiveSide()
{
    currentState.sideToMove = Color(!currentState.sideToMove);
}

void ChessBoard::makeMove(Move move)
{
    currentState.captured = pieceAt[move.to()];
    stateHistory.push(currentState);

    int from = move.from();
    PieceType piece = pieceAt[from];
    int to = move.to();
    Color side = currentState.sideToMove;
    Color enemyColor = Color(!side);
    MoveFlags flag = move.flags();
    int enPassantSquare = currentState.enPassantSquare;

    if (!move.isPromotion())
    {
        moveBit(pieceBitboards[side][piece], from, to);
        moveBit(pieceColorBitboards[side], from, to);
    }

    if (flag == QUIET_MOVE || flag == DOUBLE_PAWN_PUSH || flag == KING_CASTLE || flag == QUEEN_CASTLE || (move.isPromotion() && !move.isCapture()))
    {
        moveBit(allPieces, from, to);
        if (flag == DOUBLE_PAWN_PUSH)
            currentState.enPassantSquare = to;
    }

    if (flag == CAPTURE || (move.isPromotion() && move.isCapture()))
    {
        clearBit(pieceBitboards[enemyColor][currentState.captured], to);
        clearBit(pieceColorBitboards[enemyColor], to);
        clearBit(allPieces, from);
    }

    if (flag == EN_PASSANT)
    {
        clearBit(pieceBitboards[enemyColor][pawn], enPassantSquare);
        clearBit(pieceColorBitboards[enemyColor], enPassantSquare);
        clearBit(allPieces, enPassantSquare);
        moveBit(allPieces, from, to);
        pieceAt[enPassantSquare] = pieceType_NB;
    }

    if (move.isPromotion())
    {
        clearBit(pieceBitboards[side][pawn], from);
        clearBit(pieceColorBitboards[side], from);

        piece = move.promotionPiece();

        setBit(pieceBitboards[side][piece], to);
        setBit(pieceColorBitboards[side], to);
    }

    switch (flag)
    {
        int rookQueenSideSquare, rookKingSideSquare, rookCastlingSquare;
    case QUEEN_CASTLE:
        rookQueenSideSquare = a1 * enemyColor + a8 * side;
        rookCastlingSquare = to + 1;
        moveBit(pieceBitboards[side][rook], rookQueenSideSquare, rookCastlingSquare);
        moveBit(pieceColorBitboards[side], rookQueenSideSquare, rookCastlingSquare);
        moveBit(allPieces, rookQueenSideSquare, rookCastlingSquare);
        pieceAt[rookQueenSideSquare] = pieceType_NB;
        pieceAt[rookCastlingSquare] = rook;
        break;
    case KING_CASTLE:
        rookKingSideSquare = h1 * enemyColor + h8 * side;
        rookCastlingSquare = to - 1;
        moveBit(pieceBitboards[side][rook], rookKingSideSquare, rookCastlingSquare);
        moveBit(pieceColorBitboards[side], rookKingSideSquare, rookCastlingSquare);
        moveBit(allPieces, rookKingSideSquare, rookCastlingSquare);
        pieceAt[rookKingSideSquare] = pieceType_NB;
        pieceAt[rookCastlingSquare] = rook;
        break;
    }

    empty = ~allPieces;

    pieceAt[from] = pieceType_NB;
    pieceAt[to] = piece;

    if (enPassantSquare != -1 && flag != DOUBLE_PAWN_PUSH)
        currentState.enPassantSquare = -1;
    updateCastlingRights(side, enemyColor, from, to);
    switchActiveSide();
}

void ChessBoard::unMakeMove(Move move)
{
    currentState = stateHistory.top();
    stateHistory.pop();

    int from = move.to();
    PieceType piece = pieceAt[from];
    int to = move.from();
    Color side = currentState.sideToMove;
    Color enemyColor = Color(!side);
    MoveFlags flag = move.flags();
    int enPassantSquare = currentState.enPassantSquare;

    if (!move.isPromotion())
    {
        moveBit(pieceBitboards[side][piece], from, to);
        moveBit(pieceColorBitboards[side], from, to);
    }

    if (flag == QUIET_MOVE || flag == DOUBLE_PAWN_PUSH || flag == QUEEN_CASTLE || flag == KING_CASTLE || (move.isPromotion() && !move.isCapture()))
    {
        moveBit(allPieces, from, to);
        pieceAt[from] = pieceType_NB;
    }

    if (flag == CAPTURE || (move.isPromotion() && move.isCapture()))
    {
        setBit(pieceBitboards[enemyColor][currentState.captured], from);
        setBit(pieceColorBitboards[enemyColor], from);
        setBit(allPieces, to);
        pieceAt[from] = currentState.captured;
    }

    if (flag == EN_PASSANT)
    {
        setBit(pieceBitboards[enemyColor][pawn], enPassantSquare);
        setBit(pieceColorBitboards[enemyColor], enPassantSquare);
        setBit(allPieces, enPassantSquare);
        moveBit(allPieces, from, to);
        pieceAt[enPassantSquare] = pawn;
        pieceAt[from] = pieceType_NB;
    }

    if (move.isPromotion())
    {
        setBit(pieceBitboards[side][pawn], to);
        setBit(pieceColorBitboards[side], to);

        piece = pawn;

        clearBit(pieceBitboards[side][move.promotionPiece()], from);
        clearBit(pieceColorBitboards[side], from);
    }

    switch (flag)
    {
        int rookQueenSideSquare, rookKingSideSquare, rookCastlingSquare;
    case QUEEN_CASTLE:
        rookQueenSideSquare = a1 * enemyColor + a8 * side;
        rookCastlingSquare = from + 1;
        moveBit(pieceBitboards[side][rook], rookCastlingSquare, rookQueenSideSquare);
        moveBit(pieceColorBitboards[side], rookCastlingSquare, rookQueenSideSquare);
        moveBit(allPieces, rookCastlingSquare, rookQueenSideSquare);
        pieceAt[rookQueenSideSquare] = rook;
        pieceAt[rookCastlingSquare] = pieceType_NB;
        break;
    case KING_CASTLE:
        rookKingSideSquare = h1 * enemyColor + h8 * side;
        rookCastlingSquare = from - 1;
        moveBit(pieceBitboards[side][rook], rookCastlingSquare, rookKingSideSquare);
        moveBit(pieceColorBitboards[side], rookCastlingSquare, rookKingSideSquare);
        moveBit(allPieces, rookCastlingSquare, rookKingSideSquare);
        pieceAt[rookKingSideSquare] = rook;
        pieceAt[rookCastlingSquare] = pieceType_NB;
        break;
    }

    empty = ~allPieces;

    pieceAt[to] = piece;
}

void ChessBoard::resetChessBoard()
{
    memset(pieceBitboards, 0ULL, sizeof(pieceBitboards));
    memset(pieceColorBitboards, 0ULL, sizeof(pieceColorBitboards));
    fill(pieceAt, pieceAt + square_NB, pieceType_NB);

    stateHistory = {};
    currentState = BoardState();
}

void ChessBoard::FENPiecesPlacement(string piecesPosition)
{
    int rank = 7;
    int index = rank * 8;

    for (char notation : piecesPosition)
    {
        if (isdigit(notation))
        {
            index += (notation - '0');
            continue;
        }

        switch (notation)
        {
        case 'P':
            setBit(pieceBitboards[white][pawn], index);
            pieceAt[index] = pawn;
            break;
        case 'R':
            setBit(pieceBitboards[white][rook], index);
            pieceAt[index] = rook;
            break;
        case 'N':
            setBit(pieceBitboards[white][knight], index);
            pieceAt[index] = knight;
            break;
        case 'B':
            setBit(pieceBitboards[white][bishop], index);
            pieceAt[index] = bishop;
            break;
        case 'Q':
            setBit(pieceBitboards[white][queen], index);
            pieceAt[index] = queen;
            break;
        case 'K':
            setBit(pieceBitboards[white][king], index);
            pieceAt[index] = king;
            break;
        case 'p':
            setBit(pieceBitboards[black][pawn], index);
            pieceAt[index] = pawn;
            break;
        case 'r':
            setBit(pieceBitboards[black][rook], index);
            pieceAt[index] = rook;
            break;
        case 'n':
            setBit(pieceBitboards[black][knight], index);
            pieceAt[index] = knight;
            break;
        case 'b':
            setBit(pieceBitboards[black][bishop], index);
            pieceAt[index] = bishop;
            break;
        case 'q':
            setBit(pieceBitboards[black][queen], index);
            pieceAt[index] = queen;
            break;
        case 'k':
            setBit(pieceBitboards[black][king], index);
            pieceAt[index] = king;
            break;

        default:
            rank--;
            index = rank * 8;
            continue;
        }

        index++;
    }
}

void ChessBoard::FENActiveColor(char aspect)
{
    switch (aspect)
    {
    case 'w':
        currentState.sideToMove = white;
        break;
    case 'b':
        currentState.sideToMove = black;
        break;

    default:
        break;
    }
}

void ChessBoard::FENCastlingStates(string castlingPermision)
{
    for (char notation : castlingPermision)
    {
        switch (notation)
        {
        case 'K':
            currentState.addCastlingRights(white, true);
            break;
        case 'Q':
            currentState.addCastlingRights(white, false);
            break;
        case 'k':
            currentState.addCastlingRights(black, true);
            break;
        case 'q':
            currentState.addCastlingRights(black, false);
            break;

        default:
            currentState.removeAllCastlingRights(white);
            currentState.removeAllCastlingRights(black);
            break;
        }
    }
}

void ChessBoard::FENEnPassantAny(string enPassantString)
{
    if (enPassantString[0] == '-')
    {
        currentState.enPassantSquare = -1;
    }
    else
    {
        currentState.enPassantSquare = stringToSquareEnum.find(enPassantString)->second;
        currentState.enPassantSquare = currentState.enPassantSquare + 8 * !currentState.sideToMove - 8 * currentState.sideToMove;
    }
}

void ChessBoard::FENHalfMoveClock(string clock)
{
    halfMoveClock = stoi(clock);
}

void ChessBoard::FENFullMoveNumber(string moveNumber)
{
    fullMoveNumber = stoi(moveNumber);
}

void ChessBoard::updateCastlingRights(Color sideToMove, Color enemyColor, int from, int to)
{
    // King moves - remove all castling for that side
    if (from == e1)
        currentState.removeAllCastlingRights(white);
    else if (from == e8)
        currentState.removeAllCastlingRights(black);

    // Rook moves or an enemy rook is captured - remove corresponding castling
    int friendlyQueenSideSquare = a1 * enemyColor + a8 * sideToMove;
    int friendlyKingSideSquare = h1 * enemyColor + h8 * sideToMove;
    int enemyQueenSideSquare = a1 * sideToMove + a8 * enemyColor;
    int enemyKingSideSquare = h1 * sideToMove + h8 * enemyColor;

    if (from == friendlyQueenSideSquare)
        currentState.removeCastlingRights(sideToMove, false);
    else if (from == friendlyKingSideSquare)
        currentState.removeCastlingRights(sideToMove, true);

    if (to == enemyQueenSideSquare)
        currentState.removeCastlingRights(enemyColor, false);
    else if (to == enemyKingSideSquare)
        currentState.removeCastlingRights(enemyColor, true);
}
