#include "all_enums_header.hpp"

using namespace std;

const unordered_map<int, string> pieceSymbols = {
    {0, "P"}, {1, "N"}, {2, "B"}, {3, "R"}, { 4, "Q"}, { 5, "K"},
    {6, "p"}, {7, "n"}, {8, "b"}, {9, "r"}, {10, "q"}, {11, "k"}
};

const unordered_map<string, SquareEnum> stringToSquareEnum = 
{
    {"a1", a1}, {"b1", b1}, {"c1", c1}, {"d1", d1}, {"e1", e1}, {"f1", f1}, {"g1", g1}, {"h1", h1},
    {"a2", a2}, {"b2", b2}, {"c2", c2}, {"d2", d2}, {"e2", e2}, {"f2", f2}, {"g2", g2}, {"h2", h2},
    {"a3", a3}, {"b3", b3}, {"c3", c3}, {"d3", d3}, {"e3", e3}, {"f3", f3}, {"g3", g3}, {"h3", h3},
    {"a4", a4}, {"b4", b4}, {"c4", c4}, {"d4", d4}, {"e4", e4}, {"f4", f4}, {"g4", g4}, {"h4", h4},
    {"a5", a5}, {"b5", b5}, {"c5", c5}, {"d5", d5}, {"e5", e5}, {"f5", f5}, {"g5", g5}, {"h5", h5},
    {"a6", a6}, {"b6", b6}, {"c6", c6}, {"d6", d6}, {"e6", e6}, {"f6", f6}, {"g6", g6}, {"h6", h6},
    {"a7", a7}, {"b7", b7}, {"c7", c7}, {"d7", d7}, {"e7", e7}, {"f7", f7}, {"g7", g7}, {"h7", h7},
    {"a8", a8}, {"b8", b8}, {"c8", c8}, {"d8", d8}, {"e8", e8}, {"f8", f8}, {"g8", g8}, {"h8", h8},
};

const unordered_map<string, Color> stringToColor = 
{
    {"white", white} , {"black", black},
    {"color_NB", color_NB},
};

const string squareIndexToString[64] = 
{
    "a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1", 
    "a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
    "a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
    "a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
    "a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
    "a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
    "a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
    "a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8", 
};

const string pieceTypeToString[7] =
{
    "pawn", "knight", "bishop", "rook", "queen", "king",
    "No piece",
};

const string colorToString[3] = {
    "white", "black",
    "No color",
};