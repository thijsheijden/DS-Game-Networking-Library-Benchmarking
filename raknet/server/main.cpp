#include "iostream"
#include "server.h"
#include "structopt/app.hpp"
#include "../common/common.h"

using namespace SLNet;
using namespace std;

// parseCommandLineArguments parses the given command line arguments into a Config instance
Config parseCommandLineArguments(int argc, char *argv[]) {
    Config config;
    try
    {
        config = structopt::app("game_client").parse<Config>(argc, argv);
    }
    catch (structopt::exception &e)
    {
        std::cout << e.what() << "\n";
        std::cout << e.help();
        exit(EXIT_FAILURE);
    }

    return config;
}

int main(int argc, char *argv[]) {
    cout << "server starting up\n";

    // Seed random
    srandom(time(nullptr));

    // Create Server instance
    auto server = Server();

    // Parse given command line arguments
    auto givenConfig = parseCommandLineArguments(argc, argv);

    // Set gamestate fields
    server.gamestate.ApplyConfig(givenConfig.mapWidth.value(), givenConfig.mapHeight.value(), givenConfig.playerCount.value());

    // Create RakNet peer
    server.rakPeer = RakPeerInterface::GetInstance();
    auto startupStatus = server.rakPeer->Startup(2, new SocketDescriptor(GAME_PORT, 0), 1);
    server.rakPeer->SetMaximumIncomingConnections(givenConfig.playerCount.value());
    if (startupStatus != RAKNET_STARTED) {
        cout << "failed to start RakNet";
    }

    // Start server game loop
    server.StartGameLoop();

    return 0;
}