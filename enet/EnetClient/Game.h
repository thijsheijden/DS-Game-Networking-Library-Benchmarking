#include <cstdio>
#include <cstdlib>
#include <system_error>
#include <queue>
#include <enet/enet.h>
#include "vector"
#include "Client.h"
#include <map>
#include <fstream>
#include "../../common/corrections_tracker.h"

#ifndef CLIENT_GAME_H
#define CLIENT_GAME_H

#define GAME_ERR_BUFF_SIZE 256
class Client;
// Gamestate keeps track of the total gamestate. The state is updated both by local updates and updates received from
// the networking library

// Game tracks the gamestate and has methods to update
struct Gamestate 
{
public:
    Gamestate() = default;

    Gamestate(int numPlayers)
    {
        PositionUpdateMessage position;
        otherPlayers = std::vector<Client>(numPlayers);
        //bombs = std::vector<Bomb>(numPlayers * 3);
    };
   // void updateGameState(std::vector<Client>, std::vector<Bomb>);
private:
    std::vector<Client> otherPlayers;
    //std::vector<Bomb> bombs;
};
class Game
{
public:
    Game(int width, int height, bool reliableMessage, bool logCorrections, string correctionsLoggingDirectory);
    void startGameLoop();
    void moveLocalPlayer(PositionUpdateMessage newPos);
    //GameState game_state;
private:
    int numPlayers;
    int mapHeight;
    int mapWidth;
    char errBuff[GAME_ERR_BUFF_SIZE]{};
    int tick();
    Client* m_client = new Client();
    std::map<int, PositionUpdateMessage> m_others; 
    size_t channel = 0;
    bool countCorrections;
    corrections_tracker* correctionsTracker;
};



#endif //CLIENT_GAME_H
