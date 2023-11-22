#include "game.h"
#include "chrono"
#include "thread"
#include "game_events.h"

// startGameLoop starts the client game loop, this is a blocking call, so should not be performed on the main thread
using namespace std::chrono;
void Game::startGameLoop() {
    using Framerate = duration<steady_clock::rep, std::ratio<1, 20>>; // 20 ticks per second
    auto next = steady_clock::now() + Framerate{1};

    while (true) {
        tick();

        // Sleep until next tick
        std::this_thread::sleep_until(next);
        next += Framerate{1};
    }
}

// tick performs a single game loop tick, performing the following actions:
// 1. Choosing an action to perform
// 2. Performing this action
// 3. Passing performed action to the networking library
// 4. Process all received messages from networking library
// returns non-zero if an error has occurred
int Game::tick() {
    std::vector<std::pair<EventType, Position>> possibleActions;

    Position playerPos = gamestate.player.pos;
    if (playerPos.y >= 0.1) {
        possibleActions.emplace_back(PLAYER_MOVE_UP, Position(playerPos.x, playerPos.y - 0.1f));
    }
    if (playerPos.y <= mapHeight-0.1) {
        possibleActions.emplace_back(PLAYER_MOVE_DOWN, Position(playerPos.x, playerPos.y + 0.1f));
    }
    if (playerPos.x >= 0.1) {
        possibleActions.emplace_back(PLAYER_MOVE_LEFT, Position(playerPos.x - 0.1f, playerPos.y));
    }
    if (playerPos.x <= mapWidth-0.1) {
        possibleActions.emplace_back(PLAYER_MOVE_RIGHT, Position(playerPos.x + 0.1f, playerPos.y));
    }

    // Choose a random action to perform
    std::pair<EventType, Position> chosenAction = possibleActions[rand() % possibleActions.size()];

    // Perform the action
    gamestate.moveLocalPlayer(chosenAction.second);

    // TODO: Pass performed action to networking library
    networkLibSendQueue->emplace(chosenAction.first, PlayerMoveEvent(1, chosenAction.second));

    // Go through received updates from networking lib and process updates
    while (!(networkLibReceivedQueue->empty())){
        auto receivedUpdate = networkLibReceivedQueue->front();

        // TODO: Process update
        printf("received game update from network library of type: %u\n", receivedUpdate.first);
    }

    return 0;
}

void Gamestate::moveLocalPlayer(Position newPos) {
    player.pos = newPos;
    printf("moved local player to x: %.2f, y: %.2f\n", player.pos.x, player.pos.y);
}