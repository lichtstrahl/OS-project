cmake_minimum_required(VERSION 3.5)
project(c_client)

set(CMAKE_CXX_STANDARD 14)

add_executable(c_client main.cpp timer.h FileHolder.cpp FileHolder.h const.h Action.h color.h)