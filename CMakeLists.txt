cmake_minimum_required(VERSION 3.00)

project(square_solver VERSION 0.70)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} src/main.cpp src/ringbuffer.cpp src/utils.cpp)

find_package (Threads)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT} )






