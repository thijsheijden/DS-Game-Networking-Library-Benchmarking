/*
    Yojimbo Client Example (insecure)

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
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <time.h>
#include <signal.h>
#include "shared.h"
#include <vector>
#include <unistd.h>
#include "filesystem"
#include "../common/corrections_tracker.h"

using namespace yojimbo;
using namespace std;

bool trackCorrections = false;
corrections_tracker* correctionsTracker;

static volatile int quit = 0;

void interrupt_handler( int /*dummy*/ )
{
    quit = 1;
}

class ClientGameState: public GameState {
public:
    Player localPlayer;
    std::map<clientID, Position> remotePlayerPositions;

    void CreateLocalPlayer(clientID playerID, Position spawnPos);
    void AddRemotePlayer(clientID remotePlayerID, Position pos);
    Position PerformLocalMove();
};

struct GameConfig {
    bool GUI = false;
    bool countCorrections = false;
    string correctionsLoggingDirectory = filesystem::current_path();
    uint16_t mapWidth = 100;
    uint16_t mapHeight = 100;
    uint8_t playerCount = 2; // unused in yojimbo
    bool reliableMessaging = false;
};


// we receive info from server that new player joined the game
void AddRemotePlayer(clientID remotePlayerID, Position pos, ClientGameState& gameState) {
    gameState.remotePlayerPositions[remotePlayerID] = pos;
}

Position PerformLocalMove(ClientGameState& gameState) {
    std::vector<Position> possibleMoves;

    auto localPlayerPos = gameState.localPlayer.pos;
    if (gameState.localPlayer.pos.y > 0) possibleMoves.emplace_back(localPlayerPos.x, localPlayerPos.y - 1);
    if (gameState.localPlayer.pos.y < gameState.mapHeight) possibleMoves.emplace_back(localPlayerPos.x, localPlayerPos.y + 1);
    if (gameState.localPlayer.pos.x > 0) possibleMoves.emplace_back(localPlayerPos.x - 1, localPlayerPos.y);
    if (gameState.localPlayer.pos.x < gameState.mapWidth) possibleMoves.emplace_back(localPlayerPos.x + 1, localPlayerPos.y);

    // If the player can't perform any moves, remain at its current position
    if (possibleMoves.empty()) return localPlayerPos;

    // Choose random move
    auto chosenMove = possibleMoves[random() % possibleMoves.size()];

    // Make sure this move is not into a place someone else is currently in already
    bool validMove = false;
    while (!validMove) {
        validMove = true;
        for(auto remotePlayerPos: gameState.remotePlayerPositions) {
            if (chosenMove == remotePlayerPos.second) {
                chosenMove = possibleMoves[random() % possibleMoves.size()];
                validMove = false;
                break;
            }
        }
    }

    // Perform the move
    gameState.localPlayer.pos = chosenMove;

    return chosenMove;
}

void ProcessGameConfigMessage(GameConfigMessage* message, ClientGameState& gameState) {
    gameState.mapHeight = message->mapHeight;
    gameState.mapWidth = message->mapWidth;
    gameState.numPlayers = message->numPlayers;
    printf("Received config message: \n");
    printf("width: %d \n", gameState.mapWidth);
}

void CreateLocalPlayer(clientID clientIndex, Position spawnPosition, ClientGameState& gameState) {
    gameState.localPlayer = Player(spawnPosition);
}

// we receive our start position from server
void ProcessPlayerSpawnAndIdMessage(PlayerSpawnAndIDMessage* message, ClientGameState& gameState) {
    CreateLocalPlayer(message->playerID, Position(message->x, message->y), gameState);
    printf("received player spawn and id message: \n");
    printf("x: %d, y: %d \n", gameState.localPlayer.pos.x, gameState.localPlayer.pos.y);
}

// server decides what is our actual position after our every move
// current player updates its position according to the info form server
void ProcessNewPlayerJoinedMessage(NewPlayerJoinedMessage* message, ClientGameState& gameState) {
    AddRemotePlayer(message->playerID, Position(message->x, message->y), gameState);
    printf("\n new player! x: %d, y: %d\n", gameState.remotePlayerPositions[message->playerID].x, gameState.remotePlayerPositions[message->playerID].y);
}

void ProcessPlayerPositionMessage(PlayerPositionMessage* message, ClientGameState& gameState) {
    clientID playerToUpdate = message->playerID;

    // Check if a correction had to be made
    if (trackCorrections) {
        if (gameState.remotePlayerPositions.count(playerToUpdate) > 0) {
            auto remotePlayerPosition = gameState.remotePlayerPositions[playerToUpdate];
            if (abs(remotePlayerPosition.x - message->x) > 1 || abs(remotePlayerPosition.y - message->y) > 1) {
                correctionsTracker->correctionMade();
            }
        }
    }

    // if the current player doesn't know this new player exists
    // then new entry is created in the map
    gameState.remotePlayerPositions[playerToUpdate] = Position(message->x, message->y);
    printf("player %d updated\n", playerToUpdate);
    correctionsTracker->updatesReceivedInTick();
}

void ProcessMessage(yojimbo::Message* message, ClientGameState& gameState) {
    printf(" p ! ");
    switch (message->GetType()) {
    case (int)TestMessageType::GAME_CONFIG_MESSAGE:
        ProcessGameConfigMessage((GameConfigMessage*)message, gameState); 
        break;
    case (int)TestMessageType::PLAYER_SPAWN_AND_ID_MESSAGE:        
        ProcessPlayerSpawnAndIdMessage((PlayerSpawnAndIDMessage*)message, gameState);
        break;
    case (int)TestMessageType::NEW_PLAYER_JOINED:
        ProcessNewPlayerJoinedMessage((NewPlayerJoinedMessage*)message, gameState);
        break;        
    case (int)TestMessageType::PLAYER_POSITION_MESSAGE:
        ProcessPlayerPositionMessage((PlayerPositionMessage*)message, gameState);    
        break;
    default:
        break;
    }
}

void sendLocalPlayerMove(clientID clientIndex, Position newPos, Client& client, int channel_type) {
    PlayerMoveMessage* player_move_message = (PlayerMoveMessage*)client.CreateMessage((int)TestMessageType::PLAYER_MOVE_MESSAGE);
    player_move_message->x = newPos.x;
    player_move_message->y = newPos.y;
    player_move_message->playerID = clientIndex;
    client.SendMessage(channel_type, player_move_message);
    client.SendPackets();
}

// parseCommandLineArguments parses the given command line arguments into a Config instance
void parseCommandLineArguments(int argc, char *argv[], GameConfig* gameConfig) {
    for(;;) {
        switch(getopt(argc, argv, "grw:h:xp:")) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
            case 'g':
                gameConfig->GUI = true;
                continue;
            case 'r':
                // I use int channel_type declared in the ServerMain function in the remainder of the code
                gameConfig->reliableMessaging = true;
                continue;
            case 'w':
                gameConfig->mapWidth = atoi(optarg);
                continue;
            case 'h':
                gameConfig->mapHeight = atoi(optarg);
                continue;
            case 'x':
                gameConfig->countCorrections = true;
                continue;
            case 'p':
                gameConfig->correctionsLoggingDirectory = optarg;
                continue;
            case -1:
                break;
        }
        break;
    }
}

int ClientMain( int argc, char * argv[] )
{   
    printf( "\nconnecting client (insecure)\n" );
    double time = 100.0;
    uint64_t clientId = 0;
    random_bytes( (uint8_t*) &clientId, 8 );
    printf( "client id is %.16" PRIx64 "\n", clientId );
    GameConnectionConfig config;

    struct ClientGameState gameState;
    GameConfig gameConfig;
    parseCommandLineArguments(argc, argv, &gameConfig);
    ChannelType channel_type;
    gameConfig.reliableMessaging ? channel_type = CHANNEL_TYPE_RELIABLE_ORDERED : channel_type = CHANNEL_TYPE_UNRELIABLE_UNORDERED;
    if (gameConfig.reliableMessaging) {
        printf("running client with reliable ordered messaging\n");
    } else {
        printf("running client with unreliable unordered messaging\n");
    }

    // Allocate random buffer for the Client
    Client client( GetDefaultAllocator(), Address("0.0.0.0"), config, adapter, time );
    Address serverAddress( "127.0.0.1", ServerPort );
    uint8_t privateKey[KeyBytes];
    memset( privateKey, 0, KeyBytes );

    client.InsecureConnect( privateKey, clientId, serverAddress );

    char addressString[256];
    client.GetAddress().ToString( addressString, sizeof( addressString ) );
    printf( "client address is %s\n", addressString );

    // How much time we wait between while iterations (tick rate of 20hz)
    const double deltaTime = 0.05f;

    signal( SIGINT, interrupt_handler );

    // We don not accept any other message before we receive game config
    bool configReceived = false;
    bool playerSpawnAndIdReceived = false;

    if (gameConfig.countCorrections) {
        trackCorrections = true;
        correctionsTracker = new corrections_tracker(gameConfig.correctionsLoggingDirectory, "yojimbo");
    }

    while (!quit)
    {
        client.ReceivePackets();

        for (int i = 0; i < 2; i++) {
            yojimbo::Message* message = client.ReceiveMessage(i);
            while (message != NULL) {
                // We want to first receive config,
                // then player spawn and id
                // and then we listen for any other messages
                if (!configReceived) {
                    if (message->GetType() == (int)TestMessageType::GAME_CONFIG_MESSAGE) {
                        configReceived = true;
                        ProcessMessage(message, gameState);
                    } else {
                        printf("received message of type '%u'\n", message->GetType());
                    }
                } else {
                    if(!playerSpawnAndIdReceived) {
                        if (message->GetType() == (int)TestMessageType::PLAYER_SPAWN_AND_ID_MESSAGE) {
                            playerSpawnAndIdReceived = true;
                            ProcessMessage(message, gameState);
                        } else {
                            printf("received message of type '%u'\n", message->GetType());
                        }
                    } else {
                        ProcessMessage(message, gameState);
                    }
                }
                client.ReleaseMessage(message);
                message = client.ReceiveMessage(i);
            }
        }
        
        // once we receive config start moving on the board
        if (configReceived) {
            auto newPlayerPos = PerformLocalMove(gameState);
            printf("my new position is (%u, %u)\n", newPlayerPos.x, newPlayerPos.y);

            // Push action to the server
            sendLocalPlayerMove(clientId, newPlayerPos, client, channel_type);
        }

        correctionsTracker->writeLine();

        if ( client.IsDisconnected() )
            break;

        time += deltaTime;

        client.AdvanceTime( time );

        if ( client.ConnectionFailed() )
            break;

        yojimbo_sleep( deltaTime );
    }

    client.Disconnect();

    return 0;
}

int main( int argc, char * argv[] )
{
    if ( !InitializeYojimbo() )
    {
        printf( "error: failed to initialize Yojimbo!\n" );
        return 1;
    }

    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    srand( (unsigned int) time( NULL ) );

    int result = ClientMain( argc, argv );

    ShutdownYojimbo();

    printf( "\n" );

    return result;
}
