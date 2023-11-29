#pragma once

#include <cstdint>
#include "slikenet/peerinterface.h"
#include "slikenet/MessageIdentifiers.h"
#include <structopt/app.hpp>

#define GAME_PORT 60000
#define SERVER_IP "127.0.0.1"

// Config contains the possible command line arguments
struct Config
{
    std::optional<bool> gui = false; // Whether to show a simple GUI showing the game (NOT USED RIGHT NOW)
    std::optional<uint16_t> mapHeight = 100;
    std::optional<uint16_t> mapWidth = 100;
    std::optional<unsigned short> playerCount = 2;
};
STRUCTOPT(Config, gui, mapHeight, mapWidth, playerCount); // https://github.com/p-ranav/structopt
