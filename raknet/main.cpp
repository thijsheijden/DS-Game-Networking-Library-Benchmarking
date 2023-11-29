#include <cstdlib>
#include <libc.h>
#include <structopt/app.hpp>
#include "client/client.h"
#include "player.h"

int main(int argc, char *argv[])
{
    // Seed random generator (I know that this random generator is not the best)
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

    // Create new game client
    auto *c = new Client;

    // Create a new player
    auto *p = new Player;
    p->SetNetworkIDManager(&c->manager);
    NetworkID playerNetworkID = p->GetNetworkID();
    printf("player network ID is: %llu\n", playerNetworkID);

    auto *p2 = new Player;
    p2->SetNetworkIDManager(&c->manager);
    NetworkID p2NetworkID = p2->GetNetworkID();
    printf("player_2 network ID is: %llu\n", p2NetworkID);

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
