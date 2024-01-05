#include "iostream"
#include "slikenet/peerinterface.h"
#include "../common/common.h"
#include "client.h"
#include "ncurses.h"

using namespace SLNet;
using namespace std;
int main(int argc, char *argv[]) {
    Config config;
    parseCommandLineArguments(argc, argv, &config);

    // If we want a simple GUI
    if (config.GUI) {
        // Init ncurses
        initscr();
        cbreak();
        noecho();
        nodelay(stdscr, TRUE);
        clear();
    }

    // Create Client instance
    auto client = Client();
    if (config.reliableMessaging) {
        client.reliabilityMode = RELIABLE_ORDERED;
    }

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

    client.StartGameloop(config.GUI);

    return 0;
}