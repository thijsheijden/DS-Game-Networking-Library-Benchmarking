/*
    Shared Code for Tests and Examples.

    Copyright © 2016 - 2019, The Network Protocol Company, Inc.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

        1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
           in the documentation and/or other materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived 
           from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
    USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// shared.h

#ifndef SHARED_H
#define SHARED_H

#include "yojimbo.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <string>

using namespace yojimbo;

const int ClientPort = 30000;
const int ServerPort = 40000;

using clientID = uint8_t;
using coord = uint16_t;
using address = std::string;

class GameState {
    public:
    uint16_t mapWidth;
    uint16_t mapHeight;
    uint16_t numPlayers;

    void ApplyConfig(uint16_t _mapWidth, uint16_t _mapHeight, uint16_t _numPlayers) {
        mapWidth = _mapWidth;
        mapHeight = _mapHeight;
        numPlayers = _numPlayers;
    }
};

struct Position 
{
    public:
        coord x;
        coord y;

    Position() = default;
    Position(coord initialX, coord initialY)
    {
        x = initialX;
        y = initialY;
    }

    bool operator==(Position &p2) {
        return x == p2.x && y == p2.y;
    }
};

class Player
{
public:
    Position pos;
    uint8_t health = 3;

    Player() = default;
    Player(Position _pos) {
        pos = _pos;
    }
};

// two channels, one for each type that Yojimbo supports
enum class GameChannel {
    RELIABLE,
    UNRELIABLE,
    COUNT
};

struct GameConnectionConfig : yojimbo::ClientServerConfig {
    GameConnectionConfig()  {
        numChannels = 2;
        channel[(int)GameChannel::RELIABLE].type = yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED;
        channel[(int)GameChannel::UNRELIABLE].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
    }
};

class GameConfigMessage : public yojimbo::Message {
public:
    uint16_t mapWidth;
    uint16_t mapHeight;
    uint16_t numPlayers;

    GameConfigMessage() : mapWidth(0), mapHeight(0), numPlayers(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, mapWidth, 0, 512);
        serialize_int(stream, mapHeight, 0, 512);
        serialize_int(stream, numPlayers, 0, 512);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
};

class PlayerSpawnAndIDMessage : public yojimbo::Message {
public:
    coord x;
    coord y;
    clientID playerID;

    PlayerSpawnAndIDMessage() : x(0), y(0), playerID(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_int(stream, playerID, 0, 512);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();    
};

class NewPlayerJoinedMessage : public yojimbo::Message {
public:
    coord x;
    coord y;
    clientID playerID;

    NewPlayerJoinedMessage() : x(0), y(0), playerID(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_int(stream, playerID, 0, 512);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();    
};

class PlayerMoveMessage : public yojimbo::Message {
public:
    coord x;
    coord y;
    clientID playerID;

    PlayerMoveMessage() : x(0), y(0), playerID(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_int(stream, playerID, 0, 512);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();    
};

class PlayerPositionMessage : public yojimbo::Message {
public:
    coord x;
    coord y;
    clientID playerID;

    PlayerPositionMessage() : x(0), y(0), playerID(0) {}

    template <typename Stream>
    bool Serialize(Stream& stream) {
        serialize_int(stream, x, 0, 512);
        serialize_int(stream, y, 0, 512);
        serialize_int(stream, playerID, 0, 512);

        return true;
    }

    YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();    
};

enum TestMessageType
{
    GAME_CONFIG_MESSAGE,
    PLAYER_SPAWN_AND_ID_MESSAGE,
    NEW_PLAYER_JOINED,
    PLAYER_MOVE_MESSAGE,
    PLAYER_POSITION_MESSAGE,
    NUM_TEST_MESSAGE_TYPES
};

YOJIMBO_MESSAGE_FACTORY_START( TestMessageFactory, NUM_TEST_MESSAGE_TYPES );
YOJIMBO_DECLARE_MESSAGE_TYPE( GAME_CONFIG_MESSAGE, GameConfigMessage ); 
YOJIMBO_DECLARE_MESSAGE_TYPE( NEW_PLAYER_JOINED, NewPlayerJoinedMessage ); 
YOJIMBO_DECLARE_MESSAGE_TYPE( PLAYER_SPAWN_AND_ID_MESSAGE, PlayerSpawnAndIDMessage ); 
YOJIMBO_DECLARE_MESSAGE_TYPE( PLAYER_MOVE_MESSAGE, PlayerMoveMessage );
YOJIMBO_DECLARE_MESSAGE_TYPE( PLAYER_POSITION_MESSAGE, PlayerPositionMessage );
YOJIMBO_MESSAGE_FACTORY_FINISH();


class TestAdapter : public Adapter
{
public:

    MessageFactory * CreateMessageFactory( Allocator & allocator )
    {
        return YOJIMBO_NEW( allocator, TestMessageFactory, allocator );
    }
};

static TestAdapter adapter;

#endif // #ifndef SHARED_H
