cmake_minimum_required(VERSION 3.15)
project(Sieci_Wisielec)

set(CMAKE_CXX_STANDARD 17)

add_executable(Sieci_Wisielec main.cpp client.cpp server.cpp player.cpp player.hpp session.cpp session.hpp statuses.hpp)