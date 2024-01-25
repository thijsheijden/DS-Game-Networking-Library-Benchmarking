#include "client_gamestate.h"

using namespace std;

// CreateLocalPlayer creates the local player
void ClientGamestate::CreateLocalPlayer(NetworkID playerID, Position spawnPos) {
    localPlayer = new Player(spawnPos);
    localPlayer->SetNetworkIDManager(&networkIDManager);
    localPlayer->SetNetworkID(playerID);
}

// AddRemotePlayer adds a new remote player to the game
void ClientGamestate::AddRemotePlayer(NetworkID remotePlayerID, Position pos) {
    auto *remotePlayer = new Player(pos);
    remotePlayer->SetNetworkIDManager(&networkIDManager);
    remotePlayer->SetNetworkID(remotePlayerID);
    remotePlayerPositions.emplace_back(remotePlayerID, &remotePlayer->pos);
}

// PerformLocalMove determines the possible moves the player can make, and then performs the local move
Position ClientGamestate::PerformLocalMove() {
    vector<Position> possibleMoves;

    auto localPlayerPos = localPlayer->pos;
    if (localPlayerPos.y > 0) possibleMoves.emplace_back(localPlayerPos.x, localPlayerPos.y - 1);
    if (localPlayerPos.y < mapHeight) possibleMoves.emplace_back(localPlayerPos.x, localPlayerPos.y + 1);
    if (localPlayerPos.x > 0) possibleMoves.emplace_back(localPlayerPos.x - 1, localPlayerPos.y);
    if (localPlayerPos.x < mapWidth) possibleMoves.emplace_back(localPlayerPos.x + 1, localPlayerPos.y);

    // If the player can't perform any moves, remain at its current position
    if (possibleMoves.empty()) return localPlayerPos;

    // Choose random move
    auto chosenMove = possibleMoves[random() % possibleMoves.size()];

    // Make sure this move is not into a place someone else is currently in already
    bool validMove = false;
    while (!validMove) {
        validMove = true;
        for(auto remotePlayerPos: remotePlayerPositions) {
            if (chosenMove == *remotePlayerPos.second) {
                chosenMove = possibleMoves[random() % possibleMoves.size()];
                validMove = false;
                break;
            }
        }
    }

    // Perform the move
    localPlayer->pos = chosenMove;

    return chosenMove;
}

void ClientGamestate::Display() {
    clear();
    for (auto remotePlayerPos: remotePlayerPositions) {
        mvaddch(remotePlayerPos.second->y, remotePlayerPos.second->x, '*');
    }

    mvaddch(localPlayer->pos.y, localPlayer->pos.x, '+');
    refresh();
}