#include "server_gamestate.h"

// CreateAndSpawnNewPlayerForClient creates a new player Network object a newly connected client
void ServerGamestate::CreateAndSpawnNewPlayerForClient(SystemAddress clientAddress) {
    bool foundPosition = false;
    Position randomPos;
    while (!foundPosition) {
        // Choose a random position
        randomPos = Position(random() % mapWidth, random() % mapHeight);

        // Make sure no other player is on that position
        bool positionAlreadyOccupied = false;
        for (const auto &connectedPlayer: connectedPlayers) {
            if (connectedPlayer.second->pos == randomPos) {
                positionAlreadyOccupied = true;
                break;
            }
        }

        if (!positionAlreadyOccupied) {
            foundPosition = true;
        }
    }

    // Create connected player object and add to gamestate
    auto *player = new Player;
    player->pos = randomPos;
    player->SetNetworkIDManager(&networkIDManager);
    connectedPlayers[clientAddress] = player;
}

void ServerGamestate::RemovePlayer(SystemAddress clientAddress) {
    if (connectedPlayers.count(clientAddress)) {
        delete connectedPlayers[clientAddress];
        connectedPlayers.erase(clientAddress);
    }
}
