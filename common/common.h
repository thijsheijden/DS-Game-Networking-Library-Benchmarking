#pragma once 
#include <cstdint>

//Basic events of the game
enum class Event: uint8_t
{
  CLIENT_CONNECTED = 1,
  CLIENT_DISCONNECTED,
  BROADCAST_POSITION,
  BROADCAST_HEALTH ,
  MAP_SIZE,
  BROADCAST_STATS,
  BOMB_PLACED,
  BOMB_EXPLODED,
  UNKNOWN = 255
};

// Define entity type could be handy for messages and game logic
enum class EntityType
{
  USER,
  BOMB
};

struct Options
{
    std::optional<bool> gui = false; // Whether to show a simple GUI showing the game (NOT USED RIGHT NOW)
    std::optional<float> mapHeight = 10;
    std::optional<float> mapWidth = 10;
};

struct Bomb
{
    Position pos;
    bool explode;
    Bomb() = default;
};

struct Position
{
    float x;
    float y;

    Position()=default;
    Position(float initialX, float initialY)
    {
        x = initialX;
        y = initialY;
    }
};

// Basically all the data we need for a client to define the next tick
struct GameState
{
    float health;
    Position position;
    //other players positions
   
};

typedef struct ClientData {
  ClientData(uint32_t p_id) : health(100),id(p_id){}
  uint32_t id;
  uint32_t health;
  Position position;

};

