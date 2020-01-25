cmake_minimum_required(VERSION 3.15)
project(Sieci_Wisielec)

set(CMAKE_CXX_STANDARD 14)

add_executable(Sieci_Wisielec main.cpp client.cpp server.cpp account.cpp account.hpp player.cpp player.hpp session.cpp session.hpp statuses.hpp)