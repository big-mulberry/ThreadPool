cmake_minimum_required(VERSION 3.0)

project(threadpool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${PROJECT_SOURCE_DIR}/include)

AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/include DIR_SRCS)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

add_executable(threadpool exec/main.cpp ${DIR_SRCS})

target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic")