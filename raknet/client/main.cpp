#include "iostream"
#include "slikenet/peerinterface.h"
#include "../common/common.h"
#include "client.h"

using namespace SLNet;
using namespace std;
int main(int argc, char *argv[]) {
    cout << "hello from the client!\n";

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

    client.StartGameloop();

    return 0;
}