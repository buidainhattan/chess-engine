#include "engine_debug_helper_header.hpp"

using namespace std;

long long perftBulkCount(int depth, Engine &engine)
{
    if (depth == 0)
    {
        return 1;
    }

    long long nodes = 0;
    vector<Move> legalMoves = engine.moveGenerator.generateLegalMoves();
    if (depth == 1)
    {
        return legalMoves.size();
    }

    for (Move &move : legalMoves)
    {
        engine.chessBoard.makeMove(move);

        nodes += perftBulkCount(depth - 1, engine);

        engine.chessBoard.unMakeMove(move);
    }

    return nodes;
}

void perftBulkCountDivide(int depth, Engine &engine)
{
    if (depth <= 0)
    {
        cout << "Perft Divide depth must be greater than 0." << endl;
        return;
    }

    cout << "--- Perft Divide (Depth " << depth << ") ---" << endl;

    // These will store the SUM of all nodes/stats across ALL branches
    long long totalNodesAtDepth = 0;

    // Generate all legal moves from the current position (the root of the divide)
    std::vector<Move> rootMoves = engine.moveGenerator.generateLegalMoves();

    // Iterate through each of these root moves
    for (const auto &move : rootMoves)
    {
        // Counters for the *current branch* (reset for each root move)
        long long branchNodes = 0;

        // Make the root move to enter its subtree
        engine.chessBoard.makeMove(move);

        // Recursively call the *regular* perft function for the subtree.
        // We pass depth - 1 because we've already made one move from the root.
        branchNodes += perftBulkCount(depth - 1, engine);

        engine.chessBoard.unMakeMove(move); // Unmake the root move to restore board state for the next branch

        // Print results for this specific branch (this is the "divide" output)
        cout << squareIndexToString[move.from()] << squareIndexToString[move.to()] << ": " << branchNodes << endl;

        // Accumulate these branch results to the overall totals for the current depth
        totalNodesAtDepth += branchNodes;
    }

    // Print the final aggregated results for the entire depth
    cout << "\nTotal Results for Depth " << depth << ":" << endl;
    cout << "Nodes: " << totalNodesAtDepth << endl;
    cout << "------------------------------------------" << endl;
}

long long perft(int depth, long long &capturesCount, long long &enpassantCount, long long &castlingCount, long long &promotionsCount, long long &checksCount, long long &checkmatesCount, int previousFrom, int previousTo, Engine &engine)
{
    if (depth == 0)
    {
        return 1;
    }

    long long nodes = 0;
    vector<Move> legalMoves = engine.moveGenerator.generateLegalMoves();
    for (Move &move : legalMoves)
    {
        engine.chessBoard.makeMove(move);

        MoveFlags flag = move.flags();
        if (flag == CAPTURE || flag == EN_PASSANT)
            capturesCount++;
        if (flag == EN_PASSANT)
            enpassantCount++;
        if (flag == QUEEN_CASTLE || flag == KING_CASTLE)
            castlingCount++;
        if (move.isPromotion())
            promotionsCount++;

        nodes += perft(depth - 1,
                       capturesCount,
                       enpassantCount,
                       castlingCount,
                       promotionsCount,
                       checksCount,
                       checkmatesCount,
                       move.from(),
                       move.to(),
                       engine);

        engine.chessBoard.unMakeMove(move);
    }

    return nodes;
}

void perftDivide(int depth, Engine &engine)
{
    if (depth <= 0)
    {
        cout << "Perft Divide depth must be greater than 0." << endl;
        return;
    }

    cout << "--- Perft Divide (Depth " << depth << ") ---" << endl;

    // These will store the SUM of all nodes/stats across ALL branches
    long long totalNodesAtDepth = 0;
    long long totalCaptures = 0;
    long long totalEnpassant = 0;
    long long totalCastling = 0;
    long long totalPromotions = 0;
    long long totalChecks = 0;
    long long totalCheckmates = 0;

    // Generate all legal moves from the current position (the root of the divide)
    std::vector<Move> rootMoves = engine.moveGenerator.generateLegalMoves();

    // Iterate through each of these root moves
    for (const auto &move : rootMoves)
    {
        // Counters for the *current branch* (reset for each root move)
        long long branchNodes = 0;
        long long branchCaptures = 0;
        long long branchEnpassant = 0;
        long long branchCastling = 0;
        long long branchPromotions = 0;
        long long branchChecks = 0;
        long long branchCheckmates = 0;

        // Make the root move to enter its subtree
        engine.chessBoard.makeMove(move);

        MoveFlags flag = move.flags();
        if (flag == CAPTURE || flag == EN_PASSANT)
            branchCaptures++;
        if (flag == EN_PASSANT)
            branchEnpassant++;
        if (flag == QUEEN_CASTLE || flag == KING_CASTLE)
            branchCastling++;
        if (move.isPromotion())
            branchPromotions++;

        // Recursively call the *regular* perft function for the subtree.
        // We pass depth - 1 because we've already made one move from the root.
        branchNodes = perft(depth - 1,
                            branchCaptures,
                            branchEnpassant,
                            branchCastling,
                            branchPromotions,
                            branchChecks,
                            branchCheckmates,
                            move.from(),
                            move.to(),
                            engine);

        engine.chessBoard.unMakeMove(move); // Unmake the root move to restore board state for the next branch

        // Print results for this specific branch (this is the "divide" output)
        cout << squareIndexToString[move.from()] << squareIndexToString[move.to()] << ": "
             << "Nodes=" << branchNodes << ", "
             << "Captures=" << branchCaptures << ", "
             << "EnPassant=" << branchEnpassant << ", "
             << "Castling=" << branchCastling << ", "
             << "Promotions=" << branchPromotions << ", "
             << "Checks=" << branchChecks << ", "
             << "Checkmates=" << branchCheckmates << endl;

        // Accumulate these branch results to the overall totals for the current depth
        totalNodesAtDepth += branchNodes;
        totalCaptures += branchCaptures;
        totalEnpassant += branchEnpassant;
        totalCastling += branchCastling;
        totalPromotions += branchPromotions;
        totalChecks += branchChecks;
        totalCheckmates += branchCheckmates;
    }

    // Print the final aggregated results for the entire depth
    cout << "\nTotal Results for Depth " << depth << ":" << endl;
    cout << "Nodes: " << totalNodesAtDepth << endl;
    cout << "Captures: " << totalCaptures << endl;
    cout << "En Passant: " << totalEnpassant << endl;
    cout << "Castling: " << totalCastling << endl;
    cout << "Promotions: " << totalPromotions << endl;
    cout << "Checks: " << totalChecks << endl;
    cout << "Checkmates: " << totalCheckmates << endl;
    cout << "------------------------------------------" << endl;
}