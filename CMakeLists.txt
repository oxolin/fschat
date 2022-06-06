cmake_minimum_required(VERSION 3.5)

project(clienttcp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "~/fschat")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-pthread")
add_executable(clienttcp main.cpp)
