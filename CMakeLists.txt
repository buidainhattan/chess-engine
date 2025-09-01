# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(ChessEngine CXX) # 'CXX' specifies that this is a C++ project

# Define stand c++ version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED OUTPUT_NAME)

include(CTest)
enable_testing()

# Add an executable target
# The first argument is the name of the executable (e.g., myprogram)
# The following arguments are the source files that should be compiled
add_executable(ChessEngine
    main.cpp
    lib_chess_engine/generals/all_enums.cpp
    lib_chess_engine/helpers/bitboard_helper.cpp
    lib_chess_engine/helpers/magic_bitboard_generator.cpp
    lib_chess_engine/helpers/engine_debug_helper.cpp
    lib_chess_engine/chess_board/chess_board.cpp
    lib_chess_engine/move_generator/move_generator.cpp
    lib_chess_engine/search/search.cpp
    lib_chess_engine/engine.cpp
)
