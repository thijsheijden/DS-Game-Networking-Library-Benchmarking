#pragma once

#include "game_objects.h"
#include "slikenet/MessageIdentifiers.h"

enum EventType
{
    GAME_CONFIG = ID_USER_PACKET_ENUM, // Set the ID of our first message identifier to the last native RakNet identifier
    PLAYER_SPAWN,
    PLAYER_JOIN,
    PLAYER_LEAVE,
    PLAYER_MOVE,
    PLAYER_POSITION,
    BOMB_PLACED,
    BOMB_EXPLODED,
};

// PlayerMoveMessage denotes the player moving to a new position
#pragma pack(push, 1)
#pragma pack(pop)
struct PlayerMoveMessage {
    EventType messageType = PLAYER_MOVE;
    NetworkID playerID; // ID of the player that moved
    Position newPos; // The new position of this player

    PlayerMoveMessage(NetworkID _playerID, Position _newPos) : newPos(_newPos), playerID(_playerID) {}
};

#pragma pack(push, 1)
#pragma pack(pop)
struct PlayerPositionMessage {
    EventType messageType = PLAYER_POSITION;
    NetworkID playerID; // ID of the player whose position is being sent
    Position currentPos; // The current position of this player (this can be different than the local client expects)

    PlayerPositionMessage(NetworkID _playerID, Position _currentPos) {
        playerID = _playerID;
        currentPos = _currentPos;
    }
};

// GameConfig is the message that is sent when a client connects, containing the config for the current game
#pragma pack(push, 1)
#pragma pack(pop)
struct GameConfigMessage {
    EventType messageType = GAME_CONFIG;
    uint16_t mapWidth;
    uint16_t mapHeight;
    uint16_t playerCount;

    GameConfigMessage(uint16_t _mapWidth, uint16_t _mapHeight, uint16_t _playerCount) {
        mapWidth = _mapWidth;
        mapHeight = _mapHeight;
        playerCount = _playerCount;
    };
};

// PlayerSpawnAndIDMessage is sent when a client connects, and tells the client where to spawn its own player and the network
// ID of the player
#pragma pack(push, 1)
#pragma pack(pop)
struct PlayerSpawnAndIDMessage {
    EventType messageType = PLAYER_SPAWN;
    Position spawnPoint;
    NetworkID networkID;

    PlayerSpawnAndIDMessage(Position _spawnPoint, NetworkID _networkID) {
        spawnPoint = _spawnPoint;
        networkID = _networkID;
    }
};

// NewPlayerJoinMessage is broadcast to all clients when a new player joins
#pragma pack(push, 1)
#pragma pack(pop)
struct NewPlayerJoinMessage {
    EventType messageType = PLAYER_JOIN;
    Position spawnPoint;
    NetworkID playerID;

    NewPlayerJoinMessage(Position _spawnPoint, NetworkID _playerID) {
        spawnPoint = _spawnPoint;
        playerID = _playerID;
    }
};

// PlayerLeaveMessage is broadcast to all clients if a player leaves
#pragma pack(push, 1)
#pragma pack(pop)
struct PlayerLeaveMessage {
    EventType messageType = PLAYER_LEAVE;
    NetworkID playerID;

    PlayerLeaveMessage(NetworkID _playerID) {
        playerID = _playerID;
    }
};