cmake_minimum_required(VERSION 3.10)
project(AkademineIS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the SQLite3 amalgamation source file
add_library(sqlite3 STATIC sqlite3.c)

# Create the executable with your main.cpp
add_executable(AkademineIS main.cpp)

# Link SQLite3 library to your executable
target_link_libraries(AkademineIS sqlite3)