#include <chrono>

#include "lib_chess_engine/engine_header.hpp"
#include "lib_chess_engine/engine_debug_helper_header.hpp"

using namespace std;

int main()
{
    ChessBoard chessBoard;
    Engine engine(chessBoard);

    // chessBoard.loadFromFEN("8/2p5/3p4/KP5r/1R3pPk/8/4P3/8 b - g3 0 1");
     chessBoard.newGameSetup();
    auto start = chrono::high_resolution_clock::now();
    perftDivide(5, chessBoard, engine);
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    cout << "Time taken: " << duration.count() / 1000 << " second(s)" << endl;

    return 0;
}
