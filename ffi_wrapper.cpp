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

// C-compatible move struct with move index
struct MoveData
{
    int from_square; // 0-63 square index
    int to_square;   // 0-63 square index
    int move_type;   // 0=quiet, 1=capture, 2=enpassant, 3=castling
    int move_index;  // Index into internal moveList
};

struct MoveListResult
{
    const MoveData *moves; // Pointer to array of MoveData
    int count;
};

// Static storage for moves
static std::vector<MoveData> move_storage;
static std::vector<Move> internal_move_list;

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
    DLLEXPORT MoveListResult get_legal_moves()
    {
        move_storage.clear();
        internal_move_list = engineLite.moveGenerator.generateLegalMoves();

        for (int i = 0; i < internal_move_list.size(); i++)
        {
            const Move &move = internal_move_list[i];
            MoveData data;
            data.from_square = move.from();
            data.to_square = move.to();
            data.move_type = move.flags();
            data.move_index = i;

            move_storage.push_back(data);
        }

        MoveListResult result;
        result.moves = move_storage.data();
        result.count = move_storage.size();
        return result;
    }

    DLLEXPORT void make_move(int move_index) {
        if (move_index >= 0 && move_index < internal_move_list.size()) {
            engineLite.chessBoard.makeMove(internal_move_list[move_index]);
        }
    };

    DLLEXPORT void un_make_move(int move_index)
    {
        if (move_index >= 0 && move_index < internal_move_list.size()) {
            engineLite.chessBoard.unMakeMove(internal_move_list[move_index]);
        }
    };
}