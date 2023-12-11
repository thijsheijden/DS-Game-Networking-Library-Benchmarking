#include <cstdlib>
#include "Client.h"
#include "Game.h"
#include <optional>

 //Options contains the given configuration options
struct Options
{
    std::optional<bool> gui = std::nullopt; // Whether to show a simple GUI showing the game (NOT USED RIGHT NOW)
    std::optional<int> mapHeight = std::nullopt;
    std::optional<int> mapWidth = std::nullopt;
};


int main(int argc, char *argv[])
{
    Options opt;
    opt.gui = argc >= 2 ? (std::string(argv[1]) == "true") : false;
    opt.mapHeight = argc >= 3 ? std::stoi(argv[2]) : 10;
    opt.mapWidth = argc >= 4 ? std::stoi(argv[3]) : 10;

    Game game(opt.mapHeight.value(), opt.mapWidth.value());
    game.startGameLoop();
    return EXIT_SUCCESS;
}

