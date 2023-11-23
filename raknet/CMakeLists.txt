cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 23)

add_executable(client main.cpp
        common.h
        game_events.h
        client.h
        player.h)

target_link_libraries(client -lslikenet)