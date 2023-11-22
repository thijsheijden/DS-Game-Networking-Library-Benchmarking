cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 23)

add_executable(client main.cpp
        game.h
        game.cpp
        common.h
        game_events.h
        network_libraries/networking_library.h
        network_libraries/raknet/raknet.h
        network_libraries/raknet/raknet.cpp)
