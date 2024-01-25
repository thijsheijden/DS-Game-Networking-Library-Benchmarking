#include "server_gamestate.h"
#include "iostream"

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
    connectedPlayers.emplace(clientAddress, player);
}

void ServerGamestate::RemovePlayer(SystemAddress clientAddress) {
    if (connectedPlayers.count(clientAddress)) {
        delete connectedPlayers[clientAddress];
        connectedPlayers.erase(clientAddress);
    }
}

// UpdatePlayerPositions updates the remote player positions. It goes through the list of updated positions the players
// have sent, checks whether the move is valid (not more than 1 space), updates the player positions in the gamestate, and
// then wipes the position updates queue
void ServerGamestate::UpdatePlayerPositions() {
    for (auto updatedPlayerPos: updatedPlayerPositions) {
        // Fetch this players current position
        auto currentPlayer = networkIDManager.GET_OBJECT_FROM_ID<Player*>(updatedPlayerPos.first);

        // Player moved more than 1 space in one tick, decline position update
        if (abs(updatedPlayerPos.second.y - currentPlayer->pos.y) > 1 || abs(updatedPlayerPos.second.x - currentPlayer->pos.x) > 1) {
            currentPlayer->broadcastLocationToSelf = true; // This player needs to receive this update
            continue;
        }

        // Update player position
        currentPlayer->pos = updatedPlayerPos.second;
    }

    // Clear position updates queue
    updatedPlayerPositions.clear();
}