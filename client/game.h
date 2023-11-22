#include <cstdio>
#include <cstdlib>
#include <libc.h>
#include <system_error>
#include <queue>
#include "common.h"
#include "vector"
#include "game_events.h"
#include "network_libraries/networking_library.h"

#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#define GAME_ERR_BUFF_SIZE 256

struct Bomb
{
    Position pos;

    Bomb()= default;
};

struct Player
{
    Position pos;

    Player()= default;
};

// Gamestate keeps track of the total gamestate. The state is updated both by local updates and updates received from
// the networking library
struct Gamestate
{
public:
    Player player;

    explicit Gamestate(int numPlayers) : player()
    {
        otherPlayers = std::vector<Player>(numPlayers);
        bombs = std::vector<Bomb>(numPlayers*3);
    };

    void moveLocalPlayer(Position newPos);

private:
    std::vector<Player> otherPlayers;
    std::vector<Bomb> bombs;
};

// Game tracks the gamestate and has methods to update
class Game
{
public:
    explicit Game(
            int _numPlayers,
            float _mapHeight,
            float _mapWidth,
            queueType *networkLibReceivedQueueArg,
            queueType *networkLibSendQueueArg) :
            gamestate(_numPlayers),
            networkLibReceivedQueue(networkLibReceivedQueueArg),
            networkLibSendQueue(networkLibSendQueueArg)
    {
        numPlayers = _numPlayers;
        mapHeight = _mapHeight;
        mapWidth = _mapWidth;
    };

    void setInitialPlayerPos(Position pos)
    {
        gamestate.player.pos = pos;
        printf("%f, %f\n", gamestate.player.pos.x, gamestate.player.pos.y);
    };

    void startGameLoop();

private:
    int numPlayers;
    Gamestate gamestate;
    float mapHeight;
    float mapWidth;
    char errBuff[GAME_ERR_BUFF_SIZE]{};

    // The queue containing updates received from the networking library, to be processed in the next tick
    queueType *networkLibReceivedQueue;

    // The queue containing local updates to be sent by the networking library
    queueType *networkLibSendQueue;

    int tick();
};

#endif //CLIENT_GAME_H
