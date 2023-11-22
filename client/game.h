#include <cstdio>
#include <cstdlib>
#include <libc.h>
#include <system_error>
#include <queue>
#include "common.h"
#include "vector"
#include "game_events.h"

#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#define GAME_ERR_BUFF_SIZE 256

struct Bomb
{
    // Constructor
    Bomb()= default;

    // Fields
    Position pos;
};

struct Player
{
    // Constructor
    Player()= default;

    // Fields
    Position pos;
};

// Gamestate keeps track of the total gamestate. The state is updated both by local updates and updates received from
// the networking library
struct Gamestate
{
public:
    // Constructor
    explicit Gamestate(int numPlayers)
    {
        otherPlayers = std::vector<Player>(numPlayers);
        bombs = std::vector<Bomb>(numPlayers*3);
    };

    // Fields
    Player player{};

    // Methods
    void moveLocalPlayer(Position newPos);

private:
    // Fields
    std::vector<Player> otherPlayers;
    std::vector<Bomb> bombs;
};

// Game tracks the gamestate and has methods to update
class Game
{
public:
    // Constructor
    explicit Game(int numPlayersArg, float gameHeightArg, float gameWidthArg) : gamestate(numPlayersArg)
    {
        numPlayers = numPlayersArg;
        gameHeight = gameHeightArg;
        gameWidth = gameWidthArg;
    };

    // Methods
    void setInitialPlayerPos(Position pos)
    {
        gamestate.player.pos = pos;
        printf("%f, %f\n", gamestate.player.pos.x, gamestate.player.pos.y);
    };

    void startGameLoop();

private:
    // Fields
    int numPlayers;
    Gamestate gamestate;
    float gameHeight;
    float gameWidth;
    char errBuff[GAME_ERR_BUFF_SIZE]{};

    // Methods
    int tick();
};

#endif //CLIENT_GAME_H
