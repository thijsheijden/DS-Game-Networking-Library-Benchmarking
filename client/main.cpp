#include <cstdio>
#include <cstdlib>
#include <libc.h>
#include <system_error>
#include <structopt/app.hpp>
#include "common.h"
#include "game.h"
#include "time.h"

// Options contains the given configuration options
struct Options
{
    std::optional<bool> gui = false;
};
STRUCTOPT(Options, gui); // https://github.com/p-ranav/structopt

int main(int argc, char *argv[])
{
    // Seed random generator
    srand(time(NULL));

    // Parse given arguments into Options instance
    Options options;
    try
    {
        options = structopt::app("game_client").parse<Options>(argc, argv);
    }
    catch (structopt::exception &e)
    {
        std::cout << e.what() << "\n";
        std::cout << e.help();
    }

//    std::queue<std::pair<uint32_t, uint32_t>> test;

    // Init networking library

    // Receive game information from server (number of players, our player ID)

    // Receive our player spawn position + IDs and spawn positions of all other players

    // Init local gamestate
    Game game = Game(0, 10, 10);
    // TODO: This should be sent by the server, for now, spawn player top left
    game.setInitialPlayerPos(Position(1, 1));

    // Start game loop
    game.startGameLoop();

    return EXIT_SUCCESS;
}

/*
  Implementation requirements
  - Game loop running at certain tick rate (customizable?)
  - Random (valid) move selection and execution
  - Process received data from network lib to update own game-state
  - Pass locally performed updates to network lib
  - Simple game GUI to see things are working (draw game-state in ASCII in terminal)
*/

/*
 * Game rules:
 * - All players spawn in a random place on the map, this is decided by the server (and it prevents spawning players on top of each other
 * or too close to each other
 * - All players start with 3 hit-points
 * - Every game loop tick a player can move one field in one direction (up, down, left, right), or place a bomb down
 * - Every player has three bombs they can have placed on the field at any time
 *   - Bombs explode after 3 seconds, and have a 3x3 explosion radius
 *   - Any player caught in this radius takes 1 point of damage
*/

/*
  RakNet:
  - Allows for two types of dataframes
*/
