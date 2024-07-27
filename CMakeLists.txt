cmake_minimum_required(VERSION 3.28)
project(BouncingBall)

set(CMAKE_CXX_STANDARD 23)

add_subdirectory(libs/raylib)

add_executable(${PROJECT_NAME} src/main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)