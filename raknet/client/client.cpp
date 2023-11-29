#include "iostream"
#include "client.h"
#include "slikenet/peerinterface.h"
#include "../common/raknet_helpers.h"
#include "../common/network_messages.h"
#include "chrono"

using namespace SLNet;
using namespace std;
using namespace chrono;

// WaitForAndApplyGameConfiguration waits for the game configuration message from the server, and when this message is
// received, it applies the configuration to the local game state. For now this configuration just contains the map size
void Client::WaitForConnectionAndApplyGameConfiguration() const {
    bool configReceived = false;
    Packet *p;
    while (!configReceived) {
        p = rakPeer->Receive();
        if (p == nullptr) {sleep(1); continue;} // If we did not receive any messages

        auto pi = GetPacketIdentifier(p);
        switch (pi) {
            case ID_CONNECTION_ATTEMPT_FAILED:
                printf("failed to connect to server\n");
                exit(EXIT_FAILURE);
            case ID_CONNECTION_REQUEST_ACCEPTED:
                printf("connection with server successful\n");
                break;
            case GAME_CONFIG: {
                auto *configMessage = reinterpret_cast<GameConfigMessage*>(p->data);
                printf("received game config with map width: %d, map height: %d and %d players\n", configMessage->mapWidth,
                       configMessage->mapHeight, configMessage->playerCount);
                configReceived = true;
                break;
            }
            default:
                printf("received message of type '%u'\n", pi);
                break;
        }

        rakPeer->DeallocatePacket(p);
    }
}

void Client::WaitForPlayerSpawn() {
    bool playerSpawnMessageReceived = false;
    Packet *p;
    while (!playerSpawnMessageReceived) {
        p = rakPeer->Receive();
        if (p == nullptr) {sleep(1); continue;} // If we did not receive any messages

        auto pi = GetPacketIdentifier(p);
        switch (pi) {
            case PLAYER_SPAWN: {
                auto *playerSpawnMsg = reinterpret_cast<PlayerSpawnAndIDMessage*>(p->data);
                printf("received player spawn message\n");
                printf("player spawn position (%u, %u) with network ID %llu\n", playerSpawnMsg->spawnPoint.x, playerSpawnMsg->spawnPoint.y, playerSpawnMsg->networkID);

                localPlayer = Player(playerSpawnMsg->spawnPoint);
                localPlayer.SetNetworkIDManager(&manager);
                localPlayer.SetNetworkID(playerSpawnMsg->networkID);
                playerSpawnMessageReceived = true;
                break;
            }
            default:
                printf("received message of type '%u'\n", pi);
                break;
        }

        rakPeer->DeallocatePacket(p);
    }
}

// StartGameloop starts the client game loop
void Client::StartGameloop() {
    using Framerate = duration<steady_clock::rep, ratio<1, 20>>; // 20 ticks per second
    auto next = steady_clock::now() + Framerate{1};

    // Updates to be performed at every tick
    vector<pair<EventType, variant<PlayerMoveEvent>>> updateQueue;

    // Packet pointer
    Packet* receivedPacket;
    u_char receivedPacketIdentifier;

    // Main game loop
    while (true) {
        // Before the tick, go through received messages and queue updates to be performed
        while (steady_clock::now() < next) {
            receivedPacket = rakPeer->Receive();
            while (receivedPacket) {
                receivedPacketIdentifier = GetPacketIdentifier(receivedPacket);
                if (receivedPacketIdentifier >= ID_USER_PACKET_ENUM) {
                    // Custom packet
                    handleCustomPacket(receivedPacket, receivedPacketIdentifier);
                } else {
                    // Library packet
                    handleLibraryPacket(receivedPacket, receivedPacketIdentifier);
                }

                rakPeer->DeallocatePacket(receivedPacket);
                receivedPacket = rakPeer->Receive();
            }
        }

        // Perform queued updates
        printf("tick\n");

        // Set next time tick should occur
        next += Framerate{1};
    }
}

void Client::handleLibraryPacket(SLNet::Packet *p, u_char identifier) {

}

void Client::handleCustomPacket(SLNet::Packet *p, u_char identifier) {
    switch (identifier) {
        case PLAYER_JOIN:
            auto *newPlayerJoinMessage = reinterpret_cast<NewPlayerJoinMessage*>(p->data);
            if (newPlayerJoinMessage->playerID != localPlayer.GetNetworkID()) {
                // This player is not us, create a new player and add to gamestate

            }
            break;
    }
}