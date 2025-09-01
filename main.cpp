#include <chrono>

#include "lib_chess_engine/helpers/engine_debug_helper_header.hpp"
#include "lib_chess_engine/engine_header.hpp"

using namespace std;

int main()
{
    Engine engine;

    engine.chessBoard.newGameSetup();
    auto start = chrono::high_resolution_clock::now();
    // Move bestMove = engine.search.bestMove(4);
    // cout << "Move: " << squareIndexToString[bestMove.from()] << squareIndexToString[bestMove.to()] << endl;
    perftDivide(6, engine);
    auto end = chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    cout << "Time taken: " << duration.count() / 1000 << " second(s)" << endl;

    return 0;
}
