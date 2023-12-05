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

using namespace yojimbo;

static volatile int quit = 0;

void interrupt_handler( int /*dummy*/ )
{
    quit = 1;
}

class ClientGameState: public GameState {
public:
    Player localPlayer;
    std::map<clientID, Position> connectedPlayers;

    void CreateLocalPlayer(clientID playerID, Position spawnPos);
    void AddRemotePlayer(clientID remotePlayerID, Position pos);
    Position PerformLocalMove();
};

void AddRemotePlayer(clientID remotePlayerID, Position pos, ClientGameState& gameState) {
    gameState.connectedPlayers[remotePlayerID] = pos;
}

void ProcessTestMessage(TestMessage* message, ClientGameState& gameState) {
    // printf("message received :%d ", message->m_data);
    // printf(" Your boolean variable is: %s ", message->is_alive ? "true" : "false");
    // printf("direction: %d \n", message->direction);

    // HERE update client state
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

void ProcessPlayerSpawnAndIdMessage(PlayerSpawnAndIDMessage* message, ClientGameState& gameState) {
    CreateLocalPlayer(message->playerID, Position(message->x, message->y), gameState);
    printf("received player spawn and id message: \n");
    printf("x: %d, y: %d \n", gameState.localPlayer.pos.x, gameState.localPlayer.pos.y);
}

void ProcessNewPlayerJoinedMessage(NewPlayerJoinedMessage* message, ClientGameState& gameState) {
    AddRemotePlayer(message->playerID, Position(message->x, message->y), gameState);
    printf("\n new player! x: %d, y: %d\n", gameState.connectedPlayers[message->playerID].x, gameState.connectedPlayers[message->playerID].y);
}

// For now we only have TestMessage, message types can be added in the enum TestMessageType
// in the shared.h file, add new message type to the enum, and create a new class which
// inherits from yojimbo::Message. Remember about casting, for example (TestMessage*)message
void ProcessMessage(yojimbo::Message* message, ClientGameState& gameState) {
    printf(" p ! ");
    switch (message->GetType()) {
    case (int)TestMessageType::TEST_MESSAGE:
        ProcessTestMessage((TestMessage*)message, gameState);
        break;
    case (int)TestMessageType::GAME_CONFIG_MESSAGE:
        ProcessGameConfigMessage((GameConfigMessage*)message, gameState); 
        break;
    case (int)TestMessageType::PLAYER_SPAWN_AND_ID_MESSAGE:        
        ProcessPlayerSpawnAndIdMessage((PlayerSpawnAndIDMessage*)message, gameState);
        break;
    case (int)TestMessageType::NEW_PLAYER_JOINED:
        ProcessNewPlayerJoinedMessage((NewPlayerJoinedMessage*)message, gameState);
        break;        
    default:
        break;
    }
}

void update_message_to_send(TestMessage* message) {
    // HERE input information you want to send to the server
    // You can check which fields are in the message in the shared.h
    // class TestMessage 
    message->m_data = 42;
    message->direction = 3;
    message->is_alive = true;
}

int ClientMain( int argc, char * argv[] )
{   
    printf( "\nconnecting client (insecure)\n" );
    double time = 100.0;
    uint64_t clientId = 0;
    random_bytes( (uint8_t*) &clientId, 8 );
    printf( "client id is %.16" PRIx64 "\n", clientId );
    GameConnectionConfig config;
    // Allocate random buffer for the Client
    Client client( GetDefaultAllocator(), Address("0.0.0.0"), config, adapter, time );
    Address serverAddress( "127.0.0.1", ServerPort );

    if ( argc == 2 )
    {
        Address commandLineAddress( argv[1] );
        if ( commandLineAddress.IsValid() )
        {
            if ( commandLineAddress.GetPort() == 0 )
                commandLineAddress.SetPort( ServerPort );
            serverAddress = commandLineAddress;
        }
    }

    uint8_t privateKey[KeyBytes];
    memset( privateKey, 0, KeyBytes );

    client.InsecureConnect( privateKey, clientId, serverAddress );

    char addressString[256];
    client.GetAddress().ToString( addressString, sizeof( addressString ) );
    printf( "client address is %s\n", addressString );

    // How much time we wait between while iterations
    const double deltaTime = 0.1f;

    signal( SIGINT, interrupt_handler );

    // HERE you can initialize game state
    struct ClientGameState gameState;

    // We don not accept any other message before we receive game config
    bool configReceived = false;
    bool playerSpawnAndIdReceived = false;

    while ( !quit )
    {        

        TestMessage* message = (TestMessage*)client.CreateMessage((int)TestMessageType::TEST_MESSAGE);
        printf("Send!  ");
        update_message_to_send(message);        
        client.SendMessage((int)GameChannel::RELIABLE, message);
        client.SendPackets();


        client.ReceivePackets();

        // Process received messages
        // 2 is num of channels
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
