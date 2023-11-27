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

using namespace yojimbo;

static volatile int quit = 0;

void interrupt_handler( int /*dummy*/ )
{
    quit = 1;
}

const int MAX_CLIENTS = 64;

struct ServerGameState {
    // HERE you can update your game state on the server side
    int count;
};


void update_message_to_send(TestMessage* message) {
    // HERE input information you want to send to the server
    // You can check which fields are in the message in the shared.h
    // class TestMessage 
    message->m_data = 42;
    message->direction = 3;
    message->is_alive = true;
}

void ProcessTestMessage(int clientIndex, TestMessage* message, ServerGameState& gameState, Server& server) {
    // Read client message

    int received_count = message->m_data;
    printf("message received :%d ", received_count);
    printf(" from client % d\n", clientIndex);
    printf(" Your boolean variable is: %s ", message->is_alive ? "true" : "false");
    printf("direction: %d ", message->direction);

    // Update game state
    gameState.count += received_count;

    // Send a reply to client
    TestMessage* reply_message = (TestMessage*)server.CreateMessage(clientIndex, (int)TestMessageType::TEST_MESSAGE);
    // All fields must be initialized
    reply_message->m_data = 4;
    reply_message->direction = 4;
    reply_message->is_alive = 0;
    // This line does not send anything yet, all the messages are sent in the server.SendPackets(); line.
    server.SendMessage(clientIndex, (int)GameChannel::RELIABLE, reply_message);
}


void ProcessMessage(int clientIndex, yojimbo::Message* message, ServerGameState& gameState, Server& server) {
    printf(" processing! ");
    switch (message->GetType()) {
    case (int)TestMessageType::TEST_MESSAGE:
        ProcessTestMessage(clientIndex, (TestMessage*)message, gameState, server);
        break;
    default:
        break;
    }
}

int ServerMain()
{
    printf( "started server on port %d (insecure)\n", ServerPort );
    double time = 100.0;

    // config must be same for client and server so that they both know the channel types
    GameConnectionConfig config;

    uint8_t privateKey[KeyBytes];
    memset( privateKey, 0, KeyBytes );

    Server server( GetDefaultAllocator(), privateKey, Address( "127.0.0.1", ServerPort ), config, adapter, time );

    server.Start( MaxClients );

    char addressString[256];
    server.GetAddress().ToString( addressString, sizeof( addressString ) );
    printf( "server address is %s\n", addressString );

    // How much time we wait between while iterations
    const double deltaTime = 0.1f;

    signal( SIGINT, interrupt_handler );    

    // HERE you can initialize game state
    struct ServerGameState gameState = {0};

    while ( !quit )
    {
        server.SendPackets();

        server.ReceivePackets();
        // We iterate over all connected clients and all their channels
        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (server.IsClientConnected(i)) {
                // printf("Client %d is connected!", i);
                for (int j = 0; j < 2; j++) {
                    yojimbo::Message* message = server.ReceiveMessage(i, j);
                    while (message != NULL) {
                        // HERE processing message includes sending a response to the client
                        ProcessMessage(i, message, gameState, server);
                        printf("There is a message!");
                        server.ReleaseMessage(i, message);
                        message = server.ReceiveMessage(i, j);
                    }
                }
            }
        }


        time += deltaTime;

        server.AdvanceTime( time );

        if ( !server.IsRunning() )
            break;

        yojimbo_sleep( deltaTime );
    }

    server.Stop();

    return 0;
}

int main()
{
    printf( "\n" );

    if ( !InitializeYojimbo() )
    {
        printf( "error: failed to initialize Yojimbo!\n" );
        return 1;
    }

    yojimbo_log_level( YOJIMBO_LOG_LEVEL_INFO );

    srand( (unsigned int) time( NULL ) );

    int result = ServerMain();

    ShutdownYojimbo();

    printf( "\n" );

    return result;
}
