#pragma once 
#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
//#include <cstdint>
//#include "getopt.h"
//Basic events of the game
enum class Event
{
    POSITION_UPDATE = 5,
    RECIEVED_POSITION = 6,
    GAME_CONFIG,
    PLAYER_JOIN,
    UNKNOWN = 255
};
enum class PossibleMovements : uint8_t {
    PLAYER_MOVE_UP = 10,
    PLAYER_MOVE_DOWN,
    PLAYER_MOVE_LEFT,
    PLAYER_MOVE_RIGHT
};
#pragma once

struct Config {
    bool GUI = false;
    uint16_t mapWidth = 100;
    uint16_t mapHeight = 100;
    uint8_t playerCount = 2;
    bool reliableMessaging = false;
};


struct Parse {
     char* optarg= NULL;
     int optind = 1;
};

//Update player position
#pragma pack(push,1)
struct PositionUpdateMessage
{
    Event messageType = Event::POSITION_UPDATE;
    uint16_t x;
    uint16_t y;
    uint16_t ownerId;
    PositionUpdateMessage() = default;
    PositionUpdateMessage(uint16_t _x, uint16_t _y, uint16_t _ownerId) : x(_x), y(_y), ownerId(_ownerId) {}

};
#pragma pack(pop)

// GameConfig is the message that is sent when a client connects, containing the config for the current game
#pragma pack(push, 1)
struct GameConfigMessage {
    Event messageType = Event::GAME_CONFIG;
    uint16_t mapWidth;
    uint16_t mapHeight;
    uint16_t playerCount;
    int ownerId;
    GameConfigMessage(uint16_t _mapWidth, uint16_t _mapHeight, uint16_t _playerCount) : mapWidth(_mapWidth), mapHeight(_mapHeight), playerCount(_playerCount) {}
};
#pragma pack(pop)

// NewPlayerJoinMessage is broadcast to all clients when a new player joins
#pragma pack(push, 1)
struct NewPlayerJoinMessage {
    Event messageType = Event::PLAYER_JOIN;
    PositionUpdateMessage spawnPoint;
    uint16_t ownerId;
    NewPlayerJoinMessage(PositionUpdateMessage _spawnPoint):  spawnPoint(_spawnPoint), ownerId(_spawnPoint.ownerId) {}
};
#pragma pack(pop)

