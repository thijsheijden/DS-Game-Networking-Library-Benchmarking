#include "iostream"
#include "slikenet/peerinterface.h"
#include "../common/common.h"
#include "client.h"
#include "ncurses.h"

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

using namespace SLNet;
using namespace std;
int main(int argc, char *argv[]) {
    auto config = parseCommandLineArguments(argc, argv);

    if (config.gui.value()) {
        // Init ncurses
        initscr();
        cbreak();
        noecho();
        nodelay(stdscr, TRUE);
        clear();
    }

    // Create Client instance
    auto client = Client();

    // Create RakNet peer
    client.rakPeer = RakPeerInterface::GetInstance();
    auto startupRes = client.rakPeer->Startup(1, new SocketDescriptor, 1);
    if (startupRes != RAKNET_STARTED) {
        cout << "RakNet failed to start\n";
    }

    // Connect to server
    auto connRes = client.rakPeer->Connect(SERVER_IP, (unsigned short) GAME_PORT, 0, 0, 0, 0, 12);
    if (connRes != CONNECTION_ATTEMPT_STARTED) {
        cout << "connection failed to get started";
    }

    // Wait for successful connection and game configuration message
    client.WaitForConnectionAndApplyGameConfiguration();

    // Wait for player spawn message
    client.WaitForPlayerSpawn();

    client.StartGameloop(config.gui.value());

    return 0;
}