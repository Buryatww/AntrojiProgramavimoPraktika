cmake_minimum_required(VERSION 3.16)
project(AkademineIS VERSION 1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# SQLite3 library
add_library(sqlite3 STATIC sqlite3.c)

# Important: Set include directory for sqlite3
target_include_directories(sqlite3 PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Disable warnings for SQLite
if(MSVC)
    target_compile_options(sqlite3 PRIVATE /W0)
    target_compile_definitions(sqlite3 PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_compile_options(sqlite3 PRIVATE -w)
endif()

# Source files
set(PROJECT_SOURCES
    main.cpp
    loginwindow.h
    loginwindow.cpp
    adminwindow.h
    adminwindow.cpp
    destytojawindow.h
    destytojawindow.cpp
    studentwindow.h
    studentwindow.cpp
    models.h
    models.cpp
    repositories.h
    repositories.cpp
    exceptions.h
)

# Create executable
add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
)

# Include directories for the main executable
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    sqlite3
)

# Platform-specific libraries
if(WIN32)
    # Link Windows threading library which SQLite needs
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)
endif()
