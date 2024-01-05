#pragma once

#include <cstdint>
#include "slikenet/peerinterface.h"
#include "slikenet/MessageIdentifiers.h"

#define GAME_PORT 60000
#define SERVER_IP "127.0.0.1"

struct Config {
    bool GUI = false;
    uint16_t mapWidth = 100;
    uint16_t mapHeight = 100;
    uint8_t playerCount = 2;
    bool reliableMessaging = false;
};

// parseCommandLineArguments parses the given command line arguments into a Config instance
void parseCommandLineArguments(int argc, char *argv[], Config* config) {
    for(;;) {
        switch(getopt(argc, argv, "grc:w:h:")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
            case 'g':
                config->GUI = true;
                continue;
            case 'r':
                config->reliableMessaging = true;
                continue;
            case 'w':
                config->mapWidth = atoi(optarg);
                continue;
            case 'h':
                config->mapHeight = atoi(optarg);
                continue;
            case 'c':
                config->playerCount = atoi(optarg);
                printf("%d\n", config->playerCount);
                continue;
            case -1:
                break;
        }
        break;
    }
}