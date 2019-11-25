cmake_minimum_required(VERSION 2.8)

project(word-blitz)
find_library(UNICODE_UC icuuc)
find_library(UNICODE_IO icuio)

add_executable(${PROJECT_NAME} "main.cpp" "grid.h" "grid.cpp" "wordlist.h" "wordlist.cpp")

target_link_libraries(${PROJECT_NAME} stdc++fs ${UNICODE_IO} ${UNICODE_UC})
