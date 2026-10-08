cmake_minimum_required(VERSION 3.20)

project(padt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(padt
    src/main.cpp
    src/hypr_socket2_sock.cpp
    src/hypr_event.cpp
    src/activity.cpp
    src/activity_tracker.cpp
    src/hypr_socket_sock.cpp
)

target_include_directories(padt PRIVATE
    ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(padt PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
