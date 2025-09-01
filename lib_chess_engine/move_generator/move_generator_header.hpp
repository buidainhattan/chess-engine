#ifndef MOVE_GENERATOR_H
#define MOVE_GENERATOR_H

#include "../helpers/bitboard_helper_header.hpp"
#include "../chess_board/chess_board_header.hpp"

class MoveGenerator
{
public:
    MoveGenerator(ChessBoard &chessBoard);

    static constexpr U64 rookMagicBitBoards[64] = {36028935531679876ULL, 306246149587865603ULL, 2918350152875901592ULL, 144133055274295808ULL, 5908727246872928768ULL, 6413165451827806248ULL, 6269026074475364608ULL, 36056284813894016ULL, 140738562392165ULL, 2392812182044800ULL, 2342153418665103428ULL, 140806225133952ULL, 140771848356480ULL, 2392554482040960ULL, 4612811935816173648ULL, 10378686220497930624ULL, 36040067550035968ULL, 648941109635981312ULL, 36174482578087936ULL, 2324702934043985952ULL, 154530861814382672ULL, 1189091589977310208ULL, 180320715030792ULL, 84444692044923905ULL, 9223407773130702850ULL, 4611968051754574337ULL, 9223407223375396994ULL, 9511919111011042048ULL, 11250629588063617664ULL, 2300212703936560ULL, 18023761538322441ULL, 306666989273923840ULL, 72093053296316544ULL, 171136992019481664ULL, 17871375720448ULL, 4972114760523845632ULL, 281715511660588ULL, 202732490622832652ULL, 1152975450536937474ULL, 1126864160556037ULL, 72376727296835584ULL, 4510334148689932ULL, 2357353198420096ULL, 6922595651310714913ULL, 282677567881232ULL, 5630049390723076ULL, 562984380399649ULL, 9295539869843849220ULL, 27057331921493120ULL, 362750876773777472ULL, 1369394865724522752ULL, 2418451975064068608ULL, 198307917403853056ULL, 7205761604967465088ULL, 12754475645460054272ULL, 90424390323536384ULL, 2306513715920388742ULL, 18014673673298561ULL, 9077705454653481ULL, 10386885137107255325ULL, 79165039640849ULL, 281621685338113ULL, 17596489949700ULL, 1747467852572558338ULL};

    static constexpr int rookShifts[64] = {52, 53, 53, 53, 53, 53, 53, 52, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 53, 54, 54, 54, 54, 54, 54, 53, 52, 53, 53, 53, 53, 53, 53, 52};

    static constexpr U64 bishopMagicBitBoards[64] = {580748331483169ULL, 584974581909572ULL, 4807082021032960ULL, 1130368820314112ULL, 36594019081658385ULL, 6917811739838943232ULL, 612561069927432352ULL, 291048566254870532ULL, 74520508743352832ULL, 22527906760753728ULL, 2305865016769119232ULL, 4611695012101493776ULL, 4652222882510815232ULL, 216173898843111504ULL, 15833294126206976ULL, 35476631495169ULL, 4503620586701826ULL, 635640395990528ULL, 9227880068930375697ULL, 154389059186343936ULL, 18718206237638656ULL, 9808910365881417729ULL, 9570840984163904ULL, 18165418915155937024ULL, 1196268921690112ULL, 1153493324205212706ULL, 1518891754278092865ULL, 46303187854032930ULL, 291753213563117568ULL, 1733886406298178564ULL, 9818555316139267076ULL, 588283251182113168ULL, 2668391850224126722ULL, 4611837889564149776ULL, 563536217505824ULL, 2201172312192ULL, 6762013700133120ULL, 2307285851945632000ULL, 2306155301117790212ULL, 721702398636097600ULL, 2401069529988858880ULL, 1163058469524214784ULL, 81101147103825936ULL, 37161307484586496ULL, 8800455102532ULL, 4538818361561536ULL, 866974831353004208ULL, 9800538692820074528ULL, 2559671928128000ULL, 1153803339811520513ULL, 4900095651491087104ULL, 297308494453276694ULL, 2314929520477732880ULL, 10137518818852864ULL, 2895990490917782569ULL, 6756573076127752ULL, 585541894991265792ULL, 5476943415872724992ULL, 1125936553007176ULL, 4503651444068608ULL, 288072587815936ULL, 9223376736090325256ULL, 2884591880950513888ULL, 2312633696139616800ULL};

    static constexpr int bishopShifts[64] = {58, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 55, 55, 57, 59, 59, 59, 59, 57, 57, 57, 57, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 58, 59, 59, 59, 59, 59, 59, 58};

    U64 pawnAttacksLookupTable[color_NB][64];
    U64 knightAttacksLookupTable[64];
    U64 kingAttacksLookupTable[64];
    std::vector<U64> rookAttacksLookupTable;
    std::vector<U64> bishopAttacksLookupTable;

    std::vector<std::vector<U64>> rectangularLookupTable = std::vector<std::vector<U64>>(square_NB, std::vector<U64>(square_NB));

    // Generate legal moves of all pieces
    std::vector<Move> generateLegalMoves();

    // Generate legal moves of pawns
    void generatePawnsMoves(std::vector<Move> &legalMoves, Color sideToMove, U64 pawns, int kingSquare, U64 enemyPieces, U64 pushMask, U64 captureMask, U64 pinRay, U64 othogonalPinRay, U64 diagonalPinRay);
    // Generate legal moves of knights
    void generateKnightsMoves(std::vector<Move> &legalMoves, Color sideToMove, U64 knights, U64 enemyPieces, U64 pushMask, U64 captureMask, U64 pinRay);
    // Generate legal moves of king
    void generateKingMoves(std::vector<Move> &legalMoves, Color sideToMove, U64 king, U64 enemyPieces, U64 dangerSquares);
    // Generate legal moves of sliding pieces (rook, bishop, queen)
    void generateSlidingPiecesMoves(std::vector<Move> &legalMoves, Color sideToMove, U64 orthogonalPieces, U64 diagonalPieces, U64 enemyPieces, U64 pushMask, U64 captureMask, U64 pinRay, U64 othogonalPinRay, U64 diagonalPinRay);

    // Return pawn pseudo legal attack bitboard
    U64 getPawnPseudoLegalAttackBitBoard(int square, Color color);
    // Return knight pseudo legal attack bitboard
    U64 getKnightPseudoLegalAttackBitBoard(int square, Color color);
    // Return king pseudo legal attack bitboard
    U64 getKingPseudoLegalAttackBitBoard(int square, Color color);
    // Return rook pseudo legal attack bitboard
    U64 getRookRawAttackBitBoard(int square, U64 allPieces);
    U64 getRookPseudoLegalAttackBitBoard(int square, U64 allPieces, Color color);
    // Return bishop pseudo legal attack bitboard
    U64 getBishopRawAttackBitBoard(int square, U64 allPieces);
    U64 getBishopPseudoLegalAttackBitBoard(int square, U64 allPieces, Color color);

    // Return pieces are attacking certain square
    U64 squareAttackingBy(int square);
    U64 squareAttackingBy(int square, U64 allPieces); // Only for sliding pieces with custom allPieces bitboard

    // Return enemy pieces attacking current king
    U64 attacksToKing(int square, Color kingSide);
    U64 attacksToKing(int square, Color kingSide, U64 allPieces); // Custom all pieces bitboard
    // Return friendly piece that is absolutely pinned
    U64 absolutePinPieces(int square, Color kingColor);

    // Generate rook x-ray attacks
    U64 rookXRayAttacks(int square, U64 blockers, U64 occupancy);
    // Generate bishop x-ray attacks
    U64 bishopXRayAttacks(int square, U64 blockers, U64 occupancy);

private:
    using generateFunctionPointer = U64 (MoveGenerator::*)(int);
    using pawnsFunctonPointer = U64 (MoveGenerator::*)(U64);

    ChessBoard &chessBoard;

    // Knight's attacks mask
    U64 generateKnightAttacksMask(int square);
    // King's attacks mask
    U64 generateKingAttacksMask(int square);
    // White pawn's attacks mask
    U64 generateWhitePawnAttackMask(int square);
    // Black pawn's attacks mask
    U64 generateBlackPawnAttackMask(int square);

    // Pawns's single push, double push, En passant, attacks

    // Pawns push
    // generate white pawns forward single pushes
    U64 whitePawnSinglePush(U64 whitePawns);
    // generate white pawns forward double pushes
    U64 whitePawnDoublePush(U64 whitePawns);
    // generate black pawns forward single pushes
    U64 blackPawnSinglePush(U64 blackPawns);
    // generate black pawns forward double pushes
    U64 blackPawnDoublePush(U64 blackPawns);

    // Pawns attack
    // White pawn pseudo legal captures
    U64 whitePawnPseudoLegalCaptures(U64 whitePawns, U64 blackPieces);
    // Black pawn pseudo legal captures
    U64 blackPawnPseudoLegalCaptures(U64 blackPawns, U64 whitePieces);
    // Return squares that are heavily guarded by white pawns
    U64 whitePawnSafeSquares(U64 whitePawns, U64 blackPawns);
    // Return squares that are heavily guarded by white pawns
    U64 blackPawnSafeSquares(U64 whitePawns, U64 blackPawns);

    // All white pawns attack
    U64 whitePawnSoloAttacks(U64 whitePawns);
    U64 whitePawnDuoAttacks(U64 whitePawns);
    U64 whitePawnsAnyAttacks(U64 whitePawns);
    U64 whitePawnNEastAttacks(U64 whitePawns);
    U64 whitePawnNWestAttacks(U64 whitePawns);

    // All black pawns attack
    U64 blackPawnSoloAttacks(U64 blackPawns);
    U64 blackPawnDuoAttacks(U64 blackPawns);
    U64 blackPawnAnyAttacks(U64 blackPawns);
    U64 blackPawnSEastAttacks(U64 blackPawns);
    U64 blackPawnSWestAttacks(U64 blackPawns);

    // Generate king or knight piece lookup table
    void generateNonSlidingPieceLookupTable(U64 (&arr)[64], PieceType pieceType = knight, Color color = white);

    // Generate sliding piece lookup table
    std::vector<U64> generateSlidingPieceLookupTable(PieceType pieceType = bishop);

    // Populate rectangular in between lookup table
    void populateRectangular();
};

#endif