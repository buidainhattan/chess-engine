#include <chrono>

#include "lib_chess_engine/engine_header.hpp"

using namespace std;

// A simple function to split a string by a delimiter
vector<string> split(const string &s, char delimiter)
{
    vector<string> tokens;
    string token;
    istringstream tokenStream(s);
    while (getline(tokenStream, token, delimiter))
    {
        tokens.push_back(token);
    }
    return tokens;
}

Engine handleUCIReady()
{
    Engine engine;
    return engine;
}

// Placeholder for your chess board representation and logic
void setBoardFromFen(Engine &engine, const string FEN = startFEN)
{
    engine.chessBoard.resetChessBoard();
    engine.chessBoard.loadFromFEN(FEN);

    cout << "info string setting board to FEN " << FEN << endl;
}

void makeMove(Engine &engine, const string &moveUCI)
{
    Move moveToMake = engine.convertStringToMove(moveUCI);
    engine.chessBoard.makeMove(moveToMake);
    cout << "info string making move " << moveUCI << endl;
}

string getBestMove(Engine &engine, int level)
{
    Move moveObject = engine.search.bestMove(level);
    string moveString = squareIndexToString[moveObject.from()] + squareIndexToString[moveObject.to()];
    return moveString;
}

int main()
{
    Engine engine;
    string line;

    // The main UCI loop
    while (getline(cin, line))
    {
        // You can log commands from the GUI for debugging
        // cerr << "GUI -> Engine: " << line << endl;

        vector<string> tokens = split(line, ' ');
        if (tokens.empty())
        {
            continue;
        }

        string command = tokens[0];

        if (command == "uci")
        {
            // Respond with engine identity and options
            cout << "id name Basic Cpp UCI" << endl;
            cout << "id author You" << endl;
            // Add any UCI options your engine supports here
            cout << "uciok" << endl;
        }
        else if (command == "isready")
        {
            // GUI sends this to check if the engine is ready
            cout << "readyok" << endl;
        }
        else if (command == "ucinewgame")
        {
            // Called before a new game starts. Clear hash tables, etc.
            setBoardFromFen(engine);
        }
        else if (command == "position")
        {
            // The GUI sends the board position
            // Example: position startpos moves e2e4 e7e5
            // Example: position fen rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1

            size_t movePos = line.find("moves");
            if (tokens[1] == "startpos")
            {
                setBoardFromFen(engine);
            }
            else if (tokens[1] == "fen")
            {
                string fen;
                if (movePos != string::npos)
                {
                    fen = line.substr(13, movePos - 14); // "position fen ".length() = 13
                }
                else
                {
                    fen = line.substr(13);
                }
                setBoardFromFen(engine, fen);
            }

            if (movePos != string::npos)
            {
                string moveStr = line.substr(movePos + 6); // "moves ".length() = 6
                vector<string> moves = split(moveStr, ' ');
                for (const auto &move : moves)
                {
                    makeMove(engine, move);
                }
            }
        }
        else if (command == "go")
        {
            // The GUI wants the engine to start thinking
            // This is where you would call your search function
            string best_move = getBestMove(engine, 5);
            cout << "bestmove " << best_move << endl;
        }
        else if (command == "stop")
        {
            // GUI wants the engine to stop thinking immediately.
            // In a real engine, you'd stop the search thread.
            // For simplicity, our `go` command is instant, so we can ignore this
            // but we might send the best move found so far if the search was running.
        }
        else if (command == "quit")
        {
            // The GUI is shutting down the engine
            break; // Exit the loop and terminate
        }

        // Flush the output buffer to ensure the GUI receives the message
        cout.flush();
    }

    return 0;
}

// int main()
// {
//     Engine engine;
//     engine.chessBoard.loadFromFEN(startFEN);
//     auto start = chrono::high_resolution_clock::now();
//     Move bestMove = engine.search.bestMove(4);
//     cout << "Move: " << squareIndexToString[bestMove.from()] << squareIndexToString[bestMove.to()] << endl;
//     auto end = chrono::high_resolution_clock::now();
//     chrono::duration<double, milli> duration = end - start;
//     cout << "Time taken: " << duration.count() / 1000 << " second(s)" << endl;

//     return 0;
// }
