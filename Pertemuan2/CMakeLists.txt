cmake_minimum_required(VERSION 3.15)

# Project Name
project(LatihanParsing)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories()
include_directories(rapidjson/include)


add_executable(Pertemuan2 main.cpp)

# target_link_libraries()
#target_compile_definitions(Task2ViroseIntern PRIVATE DATA_DIR="${CMAKE_CURRENT_SOURCE_DIR}/")
