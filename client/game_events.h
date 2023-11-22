#ifndef CLIENT_GAME_EVENTS_H
#define CLIENT_GAME_EVENTS_H

#include <cstdint>
#include "common.h"

enum EventType
{
    PLAYER_MOVE_UP,
    PLAYER_MOVE_DOWN,
    PLAYER_MOVE_LEFT,
    PLAYER_MOVE_RIGHT,
    BOMB_PLACED,
    BOMB_EXPLODED,
};

// PlayerMoveEvent denotes the player moving to a new position
struct PlayerMoveEvent
{
    uint32_t playerID; // ID of the player that moved
    Position newPos; // The new position of this player

    PlayerMoveEvent(uint32_t playerIDArg, Position newPosArg) : newPos(newPosArg), playerID(playerIDArg) {}
};

#endif //CLIENT_GAME_EVENTS_H