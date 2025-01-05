cmake_minimum_required(VERSION 3.0)

project(threadpool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# include_directories(${PROJECT_SOURCE_DIR}/include)

# AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/include DIR_SRCS)
AUX_SOURCE_DIRECTORY(${PROJECT_SOURCE_DIR}/src DIR_SRCS)

add_library(threadpool SHARED ${DIR_SRCS})
target_include_directories(threadpool PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(threadpoolExec exec/main.cpp)
target_link_libraries(threadpoolExec threadpool)
target_include_directories(threadpoolExec PRIVATE ${PROJECT_SOURCE_DIR}/include)


target_compile_options(${PROJECT_NAME} PRIVATE "-Wall" "-Wextra" "-Wpedantic")