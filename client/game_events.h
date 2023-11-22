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

struct PlayerMoveEvent
{
    uint32_t playerID; // ID of the player that moved
    Position newPos; // The new position of this player
};

#endif //CLIENT_GAME_EVENTS_H