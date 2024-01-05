#include "iostream"
#include "server.h"
#include "../common/common.h"

using namespace SLNet;
using namespace std;

int main(int argc, char *argv[]) {
    cout << "server starting up\n";

    // Seed random
    srandom(time(nullptr));

    // Create Server instance
    auto server = Server();

    // Parse given command line arguments
    Config config;
    parseCommandLineArguments(argc, argv, &config);

    if (config.reliableMessaging) {
        server.reliabilityMode = RELIABLE_ORDERED;
    }

    // Set gamestate fields
    server.gamestate.ApplyConfig(config.mapWidth, config.mapHeight, config.playerCount);

    // Create RakNet peer
    server.rakPeer = RakPeerInterface::GetInstance();
    auto startupStatus = server.rakPeer->Startup(config.playerCount, new SocketDescriptor(GAME_PORT, 0), 1);
    server.rakPeer->SetMaximumIncomingConnections(config.playerCount);
    if (startupStatus != RAKNET_STARTED) {
        cout << "failed to start RakNet";
    }

    // Start server game loop
    server.StartGameLoop();

    return 0;
}