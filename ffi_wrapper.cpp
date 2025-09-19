#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <chrono>

#include "lib_chess_engine/engine_header.hpp"

// Define a macro for exporting functions from the DLL
#if defined(_MSC_VER)
// Microsoft C++ compiler
#define DLLEXPORT __declspec(dllexport)
#elif defined(__GNUC__)
// GCC or Clang
#define DLLEXPORT __attribute__((visibility("default")))
#else
// Other compilers, just define it as empty
#define DLLEXPORT
#endif

using namespace std;

// This single, persistent engine instance will hold the board state between calls.
static EngineLite engineLite;

// --- NEW STRUCT FOR RETURNING A LIST OF MOVES ---
struct MoveListResult
{
    const char **moves; // Pointer to an array of C-style strings
    int count;
};

// --- STATIC STORAGE TO KEEP MOVE STRINGS ALIVE ---
static std::vector<std::string> move_storage;
static std::vector<const char *> move_pointers;

// --- HELPER FUNCTIONS ---
void setBoardFromFen(EngineLite &engineLite, const string &FEN = startFEN)
{
    engineLite.chessBoard.loadFromFEN(FEN);
}

string getSideToMove(EngineLite &engineLite)
{
    Color sideToMove = engineLite.chessBoard.currentState.sideToMove;
    string sideToMoveString = colorToString[sideToMove];
    return sideToMoveString;
}

vector<string> getLegalMoves(EngineLite &engineLite)
{
    vector<string> moveStrings = {};
    vector<Move> moves = engineLite.moveGenerator.generateLegalMoves();
    for (Move move : moves)
    {
        string moveString = squareIndexToString[move.from()] + squareIndexToString[move.to()];
        // You'll need to add promotion char logic here if applicable
        moveStrings.push_back(moveString);
    }
    return moveStrings;
}

void makeMove(EngineLite &engineLite, const string &moveUCI)
{
    Move moveToMake = engineLite.convertStringToMove(moveUCI);
    engineLite.chessBoard.makeMove(moveToMake);
}

void unmakeMove(EngineLite &engineLite, const string &moveUCI)
{
    Move moveToUnMake = engineLite.convertStringToMove(moveUCI);
    engineLite.chessBoard.unMakeMove(moveToUnMake);
}

// --- THE EXTERN "C" WRAPPER ---
// These are the C-style functions that Dart/Flutter will call via FFI.
extern "C"
{
    DLLEXPORT void set_board_from_fen(const char *c_FEN)
    {
        string FEN = c_FEN;
        if (FEN == "")
            setBoardFromFen(engineLite);
        else
            setBoardFromFen(engineLite, FEN);
    };

    DLLEXPORT const char *get_side_to_move()
    {
        return getSideToMove(engineLite).c_str();
    }

    // Specialized function to get the list of legal moves
    DLLEXPORT MoveListResult get_legal_moves_ffi()
    {
        // 1. Generate moves using your existing C++ logic
        move_storage = getLegalMoves(engineLite);

        // 2. Create C-style string pointers that are safe to return
        move_pointers.clear();
        for (const auto &move_str : move_storage)
        {
            move_pointers.push_back(move_str.c_str());
        }

        // 3. Populate and return the struct
        MoveListResult result;
        result.count = move_pointers.size();
        result.moves = move_pointers.data();

        return result;
    }

    DLLEXPORT void make_move(const char *c_moveString)
    {
        string moveString = c_moveString;
        makeMove(engineLite, moveString);
    };

    DLLEXPORT void unmake_move(const char *c_moveString)
    {
        string moveString = c_moveString;
        unmakeMove(engineLite, moveString);
    };
}