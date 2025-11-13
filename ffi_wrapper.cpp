#include <vector>
#include <string>
#include <cstring>
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
static Engine engine;

// C-compatible move struct with move index
struct MoveData
{
    int from_square; // 0-63 square index
    int to_square;   // 0-63 square index
    int piece_promote_to = pieceType_NB;
    int move_type;  // Based on engine's flag struct
    int move_index; // Index into internal moveList
};

struct MoveListResult
{
    const MoveData *moves; // Pointer to array of MoveData
    int count;
};

// Static storage for moves
static std::vector<MoveData> move_storage;
static std::vector<Move> internal_move_list;
static char static_buffer[256];

// --- THE EXTERN "C" WRAPPER ---
// These are the C-style functions that Dart/Flutter will call via FFI.
extern "C"
{
    DLLEXPORT void set_board_from_fen(const char *c_FEN)
    {
        string FEN = c_FEN;
        if (FEN == "")
            engine.loadFromFEN(startFEN);
        else
            engine.loadFromFEN(FEN);
    };

    DLLEXPORT const char *get_side_to_move()
    {
        Color side_to_move = engine.getCurrentSide();
        string side_to_move_string = colorToString[side_to_move];
        std::strncpy(static_buffer, side_to_move_string.c_str(), 255);
        static_buffer[255] = '\0';
        return static_buffer;
    }

    // Specialized function to get the list of legal moves
    DLLEXPORT MoveListResult get_legal_moves()
    {
        move_storage.clear();
        internal_move_list = engine.getLegalMoves();

        for (int i = 0; i < internal_move_list.size(); i++)
        {
            const Move &move = internal_move_list[i];
            MoveData data;
            data.from_square = move.from();
            data.to_square = move.to();
            if (move.isPromotion())
            {
                data.piece_promote_to = move.promotionPiece();
                data.move_type = PROMOTION;
                if (move.isCapture())
                {
                    data.move_type = PROMOTION_CAPTURE;
                }
            }
            else
            {
                data.move_type = move.flags();
            }
            data.move_index = i;

            move_storage.push_back(data);
        }

        MoveListResult result;
        result.moves = move_storage.data();
        result.count = static_cast<int>(move_storage.size());
        return result;
    }

    DLLEXPORT void make_move(int move_index)
    {
        if (move_index >= 0 && move_index < internal_move_list.size())
        {
            engine.makeMove(internal_move_list[move_index]);
        }
    };

    DLLEXPORT void un_make_move(int move_index)
    {
        if (move_index >= 0 && move_index < internal_move_list.size())
        {
            engine.unMakeMove(internal_move_list[move_index]);
        }
    };

    DLLEXPORT MoveData get_best_move(int depth)
    {
        internal_move_list.clear();
        Move best_move = engine.getBestMove(depth);
        internal_move_list.push_back(best_move);
        MoveData data;
        data.from_square = best_move.from();
        data.to_square = best_move.to();
        data.move_index = 0;
        if (best_move.isPromotion())
        {
            data.piece_promote_to = best_move.promotionPiece();
            data.move_type = PROMOTION;
            if (best_move.isCapture())
            {
                data.move_type = PROMOTION_CAPTURE;
            }
        }
        else
        {
            data.move_type = best_move.flags();
        }
        return data;
    };

    DLLEXPORT int get_king_square(Color kingSide) {
        return engine.getKingSquare(kingSide);
    }

    DLLEXPORT int is_king_in_check(Color kingSide)
    {
        return (int)engine.isInCheck(kingSide);
    }

    DLLEXPORT const char *disambiguating(Color side_to_move, int move_index)
    {
        std::strncpy(static_buffer, engine.disambiguating(side_to_move, internal_move_list[move_index]).c_str(), 255);
        static_buffer[255] = '\0';
        return static_buffer;
    };
}