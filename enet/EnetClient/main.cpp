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
    Config* config = new Config();
    Parse parse;
    //parse.parseCommandLineArguments(argc, argv, config);
    Options opt;
    opt.gui = argc >= 2 ? (std::string(argv[1]) == "true") : false;
    opt.mapHeight = argc >= 3 ? std::stoi(argv[2]) : 10;
    opt.mapWidth = argc >= 4 ? std::stoi(argv[3]) : 10;
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-r") == 0) {
            config->reliableMessaging = 1;
        }
        else if (std::strcmp(argv[i], "-c") == 0)
        {
            config->correction = 0;
        }
    }
    Game game(config->mapWidth, config->mapHeight, config->reliableMessaging, config->correction);
    game.startGameLoop(); 
    return EXIT_SUCCESS;
}

