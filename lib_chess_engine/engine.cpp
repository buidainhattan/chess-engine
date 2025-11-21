#include <cassert>

#include "engine_header.hpp"

using namespace std;

void Engine::reset()
{
    chessBoard.resetChessBoard();
}

void Engine::loadFromFEN(string FEN)
{
    chessBoard.resetChessBoard();
    chessBoard.loadFromFEN(FEN);
}

Move Engine::convertStringToMove(const string &moveString)
{
    if (moveString.length() < 4 || moveString.length() > 5)
    {
        throw runtime_error("Invalid move string: length must be 4 or 5.");
    }

    int from, to;
    try
    {
        // Use .at() for safe map access, which throws an exception if the key doesn't exist.
        from = stringToSquareEnum.at(moveString.substr(0, 2));
        to = stringToSquareEnum.at(moveString.substr(2, 2));
    }
    catch (const out_of_range &)
    {
        throw runtime_error("Invalid move string: contains an invalid square.");
    }

    MoveFlags flags = QUIET_MOVE;
    PieceType moving_piece = chessBoard.pieceAt[from];

    // 1. Handle Promotions
    if (moveString.length() == 5)
    {
        // Determine the base flag (promotion or promotion with capture)
        int baseFlags = (chessBoard.pieceAt[to] != pieceType_NB) ? PROMOTION_CAPTURE : PROMOTION;

        // Determine the bits for the specific promotion piece
        int promotionPieceBits = 0;
        switch (tolower(moveString[4]))
        {
        case 'q':
            promotionPieceBits = queen - knight;
            break;
        case 'r':
            promotionPieceBits = rook - knight;
            break;
        case 'b':
            promotionPieceBits = bishop - knight;
            break;
        case 'n':
            promotionPieceBits = knight - knight;
            break;
        default:
            throw runtime_error("Invalid promotion piece: " + string(1, moveString[4]));
        }

        // Combine the base flag with the piece bits
        flags = static_cast<MoveFlags>(baseFlags | promotionPieceBits);
    }
    // 2. Handle other move types
    else
    {
        if (moving_piece == pawn)
        {
            // Double Pawn Push (a pawn moving two ranks forward)
            if (abs(to - from) == 16)
            {
                flags = DOUBLE_PAWN_PUSH;
            }
            // En Passant (diagonal pawn move to an empty square)
            else if (to == chessBoard.currentState.enPassantSquare && (abs(to - from) == 7 || abs(to - from) == 9))
            {
                flags = EN_PASSANT;
            }
        }
        else if (moving_piece == king)
        {
            // Castling (king moves two files)
            if (abs(to - from) == 2)
            {
                // If 'to' square is greater, it's a move to the right (kingside)
                flags = (to > from) ? KING_CASTLE : QUEEN_CASTLE;
            }
        }

        // A regular capture is only possible if no special flag has been set yet.
        if (flags == QUIET_MOVE && chessBoard.pieceAt[to] != pieceType_NB)
        {
            flags = CAPTURE;
        }
    }

    return Move(from, to, flags);
}

void Engine::prettyPrintBitboard()
{
    // Print top border
    cout << "  ---------------------------------" << endl;

    // Iterate through ranks from 8 (index 7) down to 1 (index 0)
    for (int rank = 7; rank >= 0; --rank)
    {
        cout << rank + 1 << " |"; // Print rank number (1-8)

        // Iterate through files from A (index 0) to H (index 7)
        for (int file = 0; file < 8; ++file)
        {
            // Calculate the 0-63 square index (A1=0, H8=63)
            int squareIndex = rank * 8 + file;
            U64 squareMask = 1ULL << squareIndex;
            string pieceChar = "-"; // Default to empty square (using a space for alignment)

            // Check if any piece occupies this square
            if (chessBoard.pieceAt[squareIndex] != pieceType_NB) {
                Color pieceColor = white;
                if (squareMask & chessBoard.pieceColorBitboards[black]) {
                    pieceColor = black;
                }
                int symbolIndex = pieceColor * 6 + chessBoard.pieceAt[squareIndex];
                pieceChar = pieceSymbols.at(symbolIndex);
            }

            // Print the piece character (using a width of 3 characters total for alignment)
            cout << " " << pieceChar << (pieceChar == " " ? " |" : " |");
        }
        cout << endl;
        cout << "  ---------------------------------" << endl;
    }

    // Print file labels (A-H)
    cout << "    A   B   C   D   E   F   G   H  " << endl;
}

BoardState Engine::getCurrentBoardState()
{
    return chessBoard.currentState;
}

Color Engine::getCurrentSide()
{
    return chessBoard.currentState.sideToMove;
}

vector<Move> Engine::getLegalMoves()
{
    return moveGenerator.generateLegalMoves();
}

void Engine::makeMove(Move move)
{
    chessBoard.makeMove(move);
}

void Engine::unMakeMove(Move move)
{
    chessBoard.unMakeMove(move);
}

Move Engine::getBestMove(int depth)
{
    return search.bestMove(depth);
}

int Engine::getKingSquare(Color kingSide)
{
    U64 kingBitboard = chessBoard.pieceBitboards[kingSide][king];
    int square = bitScanForward(kingBitboard);
    return square;
}

bool Engine::isInCheck(Color kingSide)
{
    int square = getKingSquare(kingSide);
    if (moveGenerator.attacksToKing(square, kingSide))
    {
        return true;
    }
    return false;
}

void Engine::printRepetitionTracker()
{
    for (auto pair : chessBoard.repetitionTracker) {
        cout << "{" << pair.first << " : " << pair.second << "}";
    }
}

bool Engine::isRepetition()
{
    return chessBoard.isRepetition;
}

string Engine::disambiguating(Color sideToMove, Move move)
{
    string notation;
    notation.reserve(2);
    const int from = move.from();
    PieceType pieceToMove = chessBoard.pieceAt[from];
    U64 piecesBitboard = chessBoard.pieceBitboards[sideToMove][pieceToMove];

    if (countBitBoard(piecesBitboard) <= 1)
    {
        return notation;
    }

    U64 ambiguatedMove = 0ULL;
    setBit(ambiguatedMove, move.to());
    while (piecesBitboard)
    {
        int piecePosition = bitScanForward(piecesBitboard);
        popLSB(piecesBitboard);
        U64 pieceAttackBitboard;
        switch (pieceToMove)
        {
        case knight:
            pieceAttackBitboard = moveGenerator.getKnightPseudoLegalAttackBitBoard(piecePosition, sideToMove);
            break;
        case bishop:
            pieceAttackBitboard = moveGenerator.getBishopPseudoLegalAttackBitBoard(piecePosition, sideToMove);
            break;
        case rook:
            pieceAttackBitboard = moveGenerator.getRookPseudoLegalAttackBitBoard(piecePosition, sideToMove);
            break;
        case queen:
            pieceAttackBitboard = moveGenerator.getBishopPseudoLegalAttackBitBoard(piecePosition, sideToMove) | moveGenerator.getRookPseudoLegalAttackBitBoard(piecePosition, sideToMove);
            break;
        default:
            return notation;
            break;
        }
        ambiguatedMove &= pieceAttackBitboard;
    }

    if (ambiguatedMove)
    {
        piecesBitboard = chessBoard.pieceBitboards[sideToMove][pieceToMove];
        clearBit(piecesBitboard, from);
        U64 rank = getRankBitboard(from);
        U64 file = getFileBitboard(from);
        string squareStr = squareIndexToString[from];

        if (rank & piecesBitboard)
        {
            notation.push_back(squareStr[0]);
        }
        if (file & piecesBitboard)
        {
            notation.push_back(squareStr[1]);
        }
    }

    return notation;
}

U64 Engine::getRankBitboard(const int square)
{
    return FIRST_RANK << ((square >> 3) << 3);
}

U64 Engine::getFileBitboard(const int square)
{
    return A_FILE << (square & 7);
}