#include "evaluation_header.hpp"

using namespace std;

Evaluation::Evaluation(ChessBoard &chessBoard) : chessBoard(chessBoard)
{
}

int Evaluation::evaluate()
{
    if (chessBoard.isRepetition) {
        return 0;
    }

    int whiteValue, blackValue;
    whiteValue = blackValue = 0;

    for (int pieceType = pawn; pieceType < pieceType_NB; pieceType++)
    {
        U64 whitePieceBitBoard = chessBoard.pieceBitboards[white][pieceType];
        U64 blackPieceBitBoard = chessBoard.pieceBitboards[black][pieceType];

        whiteValue += countMaterial(pieceType, countBitBoard(whitePieceBitBoard)) + scorePosition(pieceType, whitePieceBitBoard, white);
        blackValue += countMaterial(pieceType, countBitBoard(blackPieceBitBoard)) + scorePosition(pieceType, blackPieceBitBoard, black);
        ;
    }

    int perspective = chessBoard.currentState.sideToMove == white ? 1 : -1;
    return (whiteValue - blackValue) * perspective;
}

vector<Move> Evaluation::orderMoves(vector<Move> legalMoves)
{
    return vector<Move>();
}

int Evaluation::countMaterial(int pieceType, int numberOfPieces)
{
    return pieceValue[pieceType] * numberOfPieces;
}

int Evaluation::scorePosition(int pieceType, U64 pieceBitBoard, Color side)
{
    int points = 0;
    while (pieceBitBoard)
    {
        int square = side == white ? countBitBoard(getLSB(pieceBitBoard)) : getFlippedIndex(countBitBoard(getLSB(pieceBitBoard)));
        popLSB(pieceBitBoard);

        points += pieceSquareTable[pieceType][square];
    }

    return points;
}

int Evaluation::getFlippedIndex(int square)
{
    return square ^ 56;
}
