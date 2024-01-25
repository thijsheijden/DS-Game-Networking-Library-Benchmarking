cmake_minimum_required(VERSION 3.26)
project(client)

set(CMAKE_CXX_STANDARD 23)

add_executable(client client/main.cpp
        common/common.h
        client/client.h
        common/game_objects.h
        client/client.cpp
        common/network_messages.h
        common/raknet_helpers.h
        common/gamestate.cpp
        common/gamestate.h
        client/game/client_gamestate.cpp
        client/game/client_gamestate.h
        common/corrections_tracker.h
        common/corrections_tracker.cpp
)

add_executable(server server/main.cpp
        common/game_objects.h
        common/network_messages.h
        common/raknet_helpers.h
        server/server.cpp
        server/server.h
        server/game/server_gamestate.cpp
        server/game/server_gamestate.h
        common/gamestate.cpp
        common/gamestate.h
)

target_link_libraries(client -lslikenet -lncurses)
target_link_libraries(server -lslikenet)
