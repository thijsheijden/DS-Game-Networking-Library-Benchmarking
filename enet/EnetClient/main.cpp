#include <cstdlib>
#include "Client.h"
#include "Game.h"
#include <optional>
#include "filesystem"

using namespace std;

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
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "-r") == 0) {
            config->reliableMessaging = 1;
        }
        else if (std::strcmp(argv[i], "-x") == 0)
        {
            config->correction = true;
        }
    }
    Game game(config->mapWidth, config->mapHeight, config->reliableMessaging, config->correction, config->correctionsLoggingDirectory);
    game.startGameLoop(); 
    return EXIT_SUCCESS;
}

