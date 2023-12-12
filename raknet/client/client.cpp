#include "iostream"
#include "client.h"
#include "slikenet/peerinterface.h"
#include "../common/raknet_helpers.h"
#include "chrono"

using namespace SLNet;
using namespace std;
using namespace chrono;

// WaitForAndApplyGameConfiguration waits for the game configuration message from the server, and when this message is
// received, it applies the configuration to the local game state. For now this configuration just contains the map size
void Client::WaitForConnectionAndApplyGameConfiguration() {
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
                serverAddress = p->systemAddress;
                break;
            case GAME_CONFIG: {
                auto *configMessage = reinterpret_cast<GameConfigMessage*>(p->data);
                printf("received game config with map width: %d, map height: %d and %d players\n", configMessage->mapWidth,
                       configMessage->mapHeight, configMessage->playerCount);
                gamestate.ApplyConfig(configMessage->mapWidth, configMessage->mapHeight, configMessage->playerCount);
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

                gamestate.CreateLocalPlayer(playerSpawnMsg->networkID, playerSpawnMsg->spawnPoint);
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
void Client::StartGameloop(bool display) {
    using Framerate = duration<steady_clock::rep, ratio<1, 5>>; // 20 ticks per second
    auto next = steady_clock::now() + Framerate{1};

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

        // Choose action to perform locally and perform the action
        auto newPlayerPos = gamestate.PerformLocalMove();
        printf("new player position is (%u, %u)\n", newPlayerPos.x, newPlayerPos.y);

        // Push action to the server
        sendLocalPlayerMove(newPlayerPos);

        // Display
        gamestate.Display();

        // Set next time tick should occur and clear the action queue
        next += Framerate{1};
    }
}

void Client::handleLibraryPacket(SLNet::Packet *p, u_char identifier) {
    switch (identifier) {
        default:
            printf("received library packet with identifier '%u'\n", identifier);
            break;
    }
}

void Client::handleCustomPacket(SLNet::Packet *p, u_char identifier) {
    switch (identifier) {
        case PLAYER_JOIN: {
            auto *newPlayerJoinMessage = reinterpret_cast<NewPlayerJoinMessage*>(p->data);
            if (newPlayerJoinMessage->playerID != gamestate.localPlayer.GetNetworkID()) {
                // This player is not us, create a new player and add to gamestate
                gamestate.AddRemotePlayer(newPlayerJoinMessage->playerID, newPlayerJoinMessage->spawnPoint);
            }
            break;
        }
        case PLAYER_POSITION: {
            // Current player position, update local gamestate with this position
            auto *playerPositionMessage = reinterpret_cast<PlayerPositionMessage*>(p->data);
            handlePlayerPositionUpdate(playerPositionMessage->playerID, playerPositionMessage->currentPos);
            break;
        }
    }
}

// sendLocalPlayerMove sends the new position of the local player
void Client::sendLocalPlayerMove(Position newPos) {
    auto *playerMoveMessage = new PlayerMoveMessage(gamestate.localPlayer.GetNetworkID(), newPos);
    rakPeer->Send(reinterpret_cast<char*>(playerMoveMessage), sizeof(PlayerMoveMessage), HIGH_PRIORITY, RELIABLE, 0, serverAddress, false);
}

// handlePlayerPositionUpdate handles the PLAYER_POSITION message, this can also be the local player
void Client::handlePlayerPositionUpdate(NetworkID playerID, Position newPos) {
    auto player = gamestate.networkIDManager.GET_OBJECT_FROM_ID<Player*>(playerID);
    if (player == nullptr) {
        // This player is new, add them
        gamestate.AddRemotePlayer(playerID, newPos);
    } else {
        player->pos = newPos;
    }
}