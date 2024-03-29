/*
    Yojimbo Server Example (insecure)

    Copyright © 2016 - 2019, The Network Protocol Company, Inc.

    Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

        1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

        2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer 
           in the documentation and/or other materials provided with the distribution.

        3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived 
           from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
    INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR 
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
    USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "yojimbo.h"
#include <signal.h>
#include <time.h>
#include "shared.h"
// #include "server_gamestate.h"
#include <vector>
#include <cstdlib> 
#include <ctime>
#include <unistd.h>

using namespace yojimbo;
const int MAX_CLIENTS = 64;

class ServerGameState: public GameState {
public:
    std::vector<std::pair<clientID, Position>> updatedPlayerPositions;
    std::map<clientID, Player*> connectedPlayers;

    void CreateAndSpawnNewPlayerForClient(clientID clientId);
    void RemovePlayer(clientID client);
    void UpdatePlayerPositions();
};

struct GameConfig {
    bool GUI = false;
    uint16_t mapWidth = 100;
    uint16_t mapHeight = 100;
    uint8_t playerCount = 2;
    bool reliableMessaging = false;
};

// sendPlayerSpawnAndIDToClient sends the player spawn position and network ID to a newly connected client
void sendPlayerSpawnAndIDToClient(clientID clientIndex, ServerGameState& gameState, Server& server, int channel_type) {
    auto createdPlayer = gameState.connectedPlayers[clientIndex];
    PlayerSpawnAndIDMessage* player_spawn_and_id_message = (PlayerSpawnAndIDMessage*)server.CreateMessage(clientIndex, (int)TestMessageType::PLAYER_SPAWN_AND_ID_MESSAGE);
    player_spawn_and_id_message->x = createdPlayer->pos.x;
    player_spawn_and_id_message->y = createdPlayer->pos.y;
    player_spawn_and_id_message->playerID = clientIndex;
    server.SendMessage(clientIndex, channel_type, player_spawn_and_id_message);
}

// CreateAndSpawnNewPlayerForClient creates a new player Network object a newly connected client
void CreateAndSpawnNewPlayerForClient(clientID clientIndex, ServerGameState& gameState, GameConfig& gameConfig) {
    bool foundPosition = false;
    Position randomPos;
    while (!foundPosition) {
        // Choose a random position
        // randomPos = Position(3, 5);
        randomPos = Position(rand() % gameConfig.mapWidth, rand() % gameConfig.mapHeight);

        // Make sure no other player is on that position
        bool positionAlreadyOccupied = false;
        for (const auto &connectedPlayer: gameState.connectedPlayers) {
            if (connectedPlayer.second->pos == randomPos) {
                positionAlreadyOccupied = true;
                break;
            }
        }

        if (!positionAlreadyOccupied) {
            foundPosition = true;
        }
    }

    // Create connected player object and add to gamestate
    auto *player = new Player;
    player->pos = randomPos;
    gameState.connectedPlayers.emplace(clientIndex, player);
}

void broadcastNewPlayerJoinedMessage(clientID newPlayerIndex, ServerGameState& gameState, Server& server, int channel_type) {
    printf("broadcasting new player joined %d\n", newPlayerIndex);
    Position newPlayerPos = gameState.connectedPlayers[newPlayerIndex]->pos;

    for (clientID i = 0; i < MAX_CLIENTS; i++) {
        if (server.IsClientConnected(i)) {
            NewPlayerJoinedMessage* new_player_joined_message = (NewPlayerJoinedMessage*)server.CreateMessage(i, (int)TestMessageType::NEW_PLAYER_JOINED);
            new_player_joined_message->x = newPlayerPos.x;
            new_player_joined_message->y = newPlayerPos.y;
            new_player_joined_message->playerID = newPlayerIndex;
            server.SendMessage(i, channel_type, new_player_joined_message);
        }
    }
}

void removePlayer(clientID playerIndex, ServerGameState& gameState) {
    printf("\nPlayer removed from board\n");
    if (gameState.connectedPlayers.count(playerIndex)) {
        delete gameState.connectedPlayers[playerIndex];
        gameState.connectedPlayers.erase(playerIndex);
    }
}

void ServerGameState::UpdatePlayerPositions() {
    for (std::pair<clientID, Position> updatedPlayerPos: updatedPlayerPositions) {
        // Fetch this players current position
        if(connectedPlayers.count(updatedPlayerPos.first) == 0)
            continue;

        Player* currentPlayer = connectedPlayers[updatedPlayerPos.first];

        // Player moved more than 1 space in one tick, decline position update
        if (abs(updatedPlayerPos.second.y - currentPlayer->pos.y) > 1 || abs(updatedPlayerPos.second.x - currentPlayer->pos.x) > 1) continue;

        // Update player position
        currentPlayer->pos = updatedPlayerPos.second;
        printf("player %d moved, x: %d, y: %d\n", updatedPlayerPos.first, currentPlayer->pos.x, currentPlayer->pos.y);
    }

    // Clear position updates queue
    updatedPlayerPositions.clear();
}

// sendGameConfigToClient sends the game config (map size, player count etc) to a newly connected client
void sendGameConfigToClient(clientID clientIndex, Server& server, ServerGameState& gameState, GameConfig& gameConfig, int channel_type) {
    printf("send game config to client %d\n", clientIndex);

    GameConfigMessage* config_message = (GameConfigMessage*)server.CreateMessage(clientIndex, (int)TestMessageType::GAME_CONFIG_MESSAGE);
    config_message->mapHeight = gameConfig.mapHeight;
    config_message->mapWidth = gameConfig.mapWidth;
    config_message->numPlayers = gameState.numPlayers;
    server.SendMessage(clientIndex, channel_type, config_message);
}

void broadcastUpdatedPlayerPosition(clientID updatedPlayerId, Server& server, ServerGameState& gameState, int channel_type) {
    printf("sending updated client position");

    for (clientID i = 0; i < MAX_CLIENTS; i++) {
        if (server.IsClientConnected(i)) {
            PlayerPositionMessage* position_message = (PlayerPositionMessage*)server.CreateMessage(i, (int)TestMessageType::PLAYER_POSITION_MESSAGE);
            position_message->x = gameState.connectedPlayers[updatedPlayerId]->pos.x;
            position_message->y = gameState.connectedPlayers[updatedPlayerId]->pos.y;
            position_message->playerID = updatedPlayerId;
            server.SendMessage(i, channel_type, position_message);
        }
    }
}

// parseCommandLineArguments parses the given command line arguments into a Config instance
void parseCommandLineArguments(int argc, char *argv[], GameConfig* config) {
    for(;;) {
        switch(getopt(argc, argv, "grw:h:")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
            case 'g':
                config->GUI = true;
                continue;
            case 'r':
                // I use int channel_type declared in the ServerMain function in the remainder of the code
                config->reliableMessaging = true;
                continue;
            case 'w':
                config->mapWidth = atoi(optarg);
                continue;
            case 'h':
                config->mapHeight = atoi(optarg);
                continue;
            case -1:
                break;
        }
        break;
    }
}

static volatile int quit = 0;


void interrupt_handler( int /*dummy*/ )
{
    quit = 1;
}

void ProcessPlayerMoveMessage(clientID clientIndex, PlayerMoveMessage* message, ServerGameState& gameState) {
    printf("player %d moved received, x: %d, y: %d\n", clientIndex, message->x, message->y);
    gameState.updatedPlayerPositions.push_back({message->playerID, Position(message->x, message->y)});
}

void ProcessMessage(clientID clientIndex, yojimbo::Message* message, ServerGameState& gameState) {
    printf(" processing! ");
    switch (message->GetType()) {
    case (int)TestMessageType::PLAYER_MOVE_MESSAGE:
        ProcessPlayerMoveMessage(clientIndex, (PlayerMoveMessage*)message, gameState);
        break;    
    default:
        break;
    }
}

int ServerMain(int argc, char * argv[])
{
    printf( "started server on port %d (insecure)\n", ServerPort );
    double time = 100.0;


    // config must be same for client and server so that they both know the channel types
    GameConnectionConfig config;
    ServerGameState gameState;
    GameConfig gameConfig;
    parseCommandLineArguments(argc, argv, &gameConfig);
    ChannelType channel_type;
    gameConfig.reliableMessaging ? channel_type = CHANNEL_TYPE_RELIABLE_ORDERED : channel_type = CHANNEL_TYPE_UNRELIABLE_UNORDERED;
    printf("%d\n", channel_type);
    if (gameConfig.reliableMessaging) {
        printf("running server with reliable ordered messaging\n");
    } else {
        printf("running server with unreliable unordered messaging\n");
    }
    printf("map height: %d, map width: %d\n", gameConfig.mapHeight, gameConfig.mapWidth);

    uint8_t privateKey[KeyBytes];
    memset( privateKey, 0, KeyBytes );

    Server server( GetDefaultAllocator(), privateKey, Address( "127.0.0.1", ServerPort ), config, adapter, time );

    server.Start( MaxClients );

    char addressString[256];
    server.GetAddress().ToString( addressString, sizeof( addressString ) );
    printf( "server address is %s\n", addressString );

    // How much time we wait between while iterations (20hz)
    const double deltaTime = 0.05f;

    signal( SIGINT, interrupt_handler );    

    while ( !quit )
    {
        server.SendPackets();

        server.ReceivePackets();

        // handle new clients connected
        for (clientID i = 0; i < MAX_CLIENTS; i++) {
            if (server.IsClientConnected(i)) {
                if (gameState.connectedPlayers.find(i) == gameState.connectedPlayers.end()) {
                    sendGameConfigToClient(i, server, gameState, gameConfig, channel_type);
                    CreateAndSpawnNewPlayerForClient(i, gameState, gameConfig);
                    sendPlayerSpawnAndIDToClient(i, gameState, server, channel_type);
                    broadcastNewPlayerJoinedMessage(i, gameState, server, channel_type);
                }
            } else {
                // client disconnected but still on the board
                if (gameState.connectedPlayers.find(i) != gameState.connectedPlayers.end()) {
                    removePlayer(i, gameState);
                }
            }
        }

        // We iterate over all connected clients and all their channels
        for (clientID i = 0; i < MAX_CLIENTS; i++) {
            if (server.IsClientConnected(i)) {
                // printf("Client %d is connected!", i);
                for (int j = 0; j < 2; j++) {
                    yojimbo::Message* message = server.ReceiveMessage(i, j);
                    while (message != NULL) {
                        ProcessMessage(i, message, gameState);
                        server.ReleaseMessage(i, message);
                        message = server.ReceiveMessage(i, j);
                    }
                }
            }
        }

        // Perform queued position updates
        gameState.UpdatePlayerPositions();

        // Broadcast updated player positions
        for (clientID i = 0; i < MAX_CLIENTS; i++) {
            if (server.IsClientConnected(i)) {
                broadcastUpdatedPlayerPosition(i, server, gameState, channel_type);
            }
        }

        time += deltaTime;

        // This is the line where they check connected clients
        server.AdvanceTime( time );

        if ( !server.IsRunning() )
            break;

        yojimbo_sleep( deltaTime );
    }

    server.Stop();

    return 0;
}

int main(int argc, char * argv[] )
{
    printf( "\n" );

    if ( !InitializeYojimbo() )
    {
        printf( "error: failed to initialize Yojimbo!\n" );
        return 1;
    }

    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    srand( (unsigned int) time( NULL ) );

    int result = ServerMain(argc, argv);

    ShutdownYojimbo();

    printf( "\n" );

    return result;
}
