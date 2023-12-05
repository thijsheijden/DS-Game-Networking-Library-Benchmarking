#pragma once 
#include <iostream>
#include <cstdint>
#include <string>
#include <sstream>
//Basic events of the game
enum class Event
{
  BROADCAST_POSITION = 5,
  RECIEVED_POSITION = 6,
  BROADCAST_HEALTH ,
  MAP_SIZE,
  BROADCAST_STATS,
  BOMB_PLACED,
  BOMB_EXPLODED,
  UNKNOWN = 255
};
enum class PossibleMovements : uint8_t {
    PLAYER_MOVE_UP = 10,
    PLAYER_MOVE_DOWN,
    PLAYER_MOVE_LEFT,
    PLAYER_MOVE_RIGHT
};
// Define entity type could be handy for messages and game logic
enum class EntityType
{
  USER,
  BOMB
};

//struct Options
//{
//    std::optional<bool> gui = false; // Whether to show a simple GUI showing the game (NOT USED RIGHT NOW)
//    std::optional<float> mapHeight = 10;
//    std::optional<float> mapWidth = 10;
//};

struct Position
{
    int x;
    int y;
    int opt_code;
    int ownerId;
    Position()=default;
    Position(int initialX, int initialY)
    {
        x = initialX;
        y = initialY;
    }
    std::string serialize(int peerID) const {
        return std::to_string(peerID) + "," + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(opt_code);
    }

    // Deserialization method
    void deserialize(const std::string& data, int &id) {
        std::istringstream ss(data);
        char dummy;
        try 
        {
            if (ss >> ownerId >> dummy >>  x >> dummy >> y >> dummy >> id)
            {
                return;
            }
            else
            {
                throw std::invalid_argument("Error parsing data.");
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        
    }
};

struct Bomb
{
    Position pos;
    bool explode;
    Bomb() = default;
};



// Basically all the data we need for a client to define the next tick
struct GameState
{
    float health;
    Position position;
    //other players positions
   
};


