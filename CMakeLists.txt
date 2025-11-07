# Set the minimum required version of CMake
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(ChessEngine CXX)

# Define C++ standard version
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- SET GLOBAL OUTPUT DIRECTORIES ---
# This is the new, more robust method.
# It tells CMake to configure the Visual Studio project to place all
# DLLs and EXEs into a "bin" directory.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# --- Build option to switch between EXE and DLL ---
option(BUILD_SHARED_LIB "Build a shared library (.dll) for FFI" OFF)

# --- Define your list of source files ---
set(ENGINE_SOURCES
    lib_chess_engine/generals/all_enums.cpp
    lib_chess_engine/helpers/bitboard_helper.cpp
    lib_chess_engine/chess_board/chess_board.cpp
    lib_chess_engine/move_generator/move_generator.cpp
    lib_chess_engine/engine.cpp
    lib_chess_engine/search/search.cpp
    lib_chess_engine/evaluation/evaluation.cpp
)

set(HELPER_SOURCES
    lib_chess_engine/helpers/magic_bitboard_generator.cpp
    lib_chess_engine/helpers/engine_debug_helper.cpp
)

# Build the SHARED LIBRARY (.dll)
add_library(ChessEnginex64 SHARED
    ffi_wrapper.cpp
    ${ENGINE_SOURCES}
)

# Build the EXECUTABLE
add_executable(ChessEngine
    main.cpp
    ${ENGINE_SOURCES}
    ${HELPER_SOURCES}
)

