#include "server.h"
#include "../common/raknet_helpers.h"
#include "slikenet/peerinterface.h"
#include "../common/network_messages.h"

using namespace std;
using namespace chrono;

// StartGameLoop starts the server game loop, this is a blocking call, so should either be the last thing called in main
// or be started in a thread
void Server::StartGameLoop() {
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

        // Perform queued position updates
        gamestate.UpdatePlayerPositions();

        // Broadcast updated player positions
        for (auto player: gamestate.connectedPlayers) {
            auto *playerPositionMessage = new PlayerPositionMessage(player.second->GetNetworkID(), player.second->pos);
            rakPeer->Send(reinterpret_cast<char*>(playerPositionMessage), sizeof(PlayerPositionMessage), HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
        }

        // Set next time tick should occur
        next += Framerate{1};
    }
}

void Server::handleLibraryPacket(Packet *p, u_char identifier) {
    switch (identifier) {
        case ID_NEW_INCOMING_CONNECTION:
            printf("new client connecting from %s\n", p->systemAddress.ToString());
            sendGameConfigToClient(p->systemAddress);
            gamestate.CreateAndSpawnNewPlayerForClient(p->systemAddress);
            sendPlayerSpawnAndIDToClient(p->systemAddress);
            broadcastNewPlayerJoinedMessage(gamestate.connectedPlayers[p->systemAddress]);
            break;
        case ID_CONNECTION_LOST:
            printf("connection to client %s lost\n", p->systemAddress.ToString());
            gamestate.RemovePlayer(p->systemAddress);
            break;
    }
}

void Server::handleCustomPacket(Packet *p, u_char identifier) {
    switch (identifier) {
        case PLAYER_MOVE:
            auto *playerMoveMessage = reinterpret_cast<PlayerMoveMessage*>(p->data);
            gamestate.updatedPlayerPositions.emplace_back(playerMoveMessage->playerID, playerMoveMessage->newPos);
            break;
    }
}

// sendGameConfigToClient sends the game config (map size, player count etc) to a newly connected client
void Server::sendGameConfigToClient(SystemAddress address) const {
    auto *gameConfigMessage = new GameConfigMessage(gamestate.mapWidth, gamestate.mapHeight, gamestate.numPlayers);
    rakPeer->Send(reinterpret_cast<char*>(gameConfigMessage), sizeof(GameConfigMessage), HIGH_PRIORITY, RELIABLE, 0, address, false);
}

// sendPlayerSpawnAndIDToClient sends the player spawn position and network ID to a newly connected client
void Server::sendPlayerSpawnAndIDToClient(SystemAddress address) {
    auto createdPlayer = gamestate.connectedPlayers[address];
    auto *playerSpawnAndIDMessage = new PlayerSpawnAndIDMessage(createdPlayer->pos, createdPlayer->GetNetworkID());
    rakPeer->Send(reinterpret_cast<char*>(playerSpawnAndIDMessage), sizeof(PlayerSpawnAndIDMessage), HIGH_PRIORITY, RELIABLE, 0, address, false);
}

// broadcastNewPlayerJoinedMessage broadcasts to all connected clients when a new player joins the game
void Server::broadcastNewPlayerJoinedMessage(Player *player) {
    auto *newPlayerJoinedMessage = new NewPlayerJoinMessage(player->pos, player->GetNetworkID());
    rakPeer->Send(reinterpret_cast<char*>(newPlayerJoinedMessage), sizeof(NewPlayerJoinMessage), HIGH_PRIORITY, RELIABLE, 0, UNASSIGNED_SYSTEM_ADDRESS, true);
}