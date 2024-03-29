#include "Game.h"
#include "chrono"
#include "thread"
#include <filesystem>
using namespace std::chrono;
#define MAX_POSITION_CORRECTION 2
#pragma region Packeting
namespace Packet
{
	decltype(auto) send_packet = [](ENetPeer* peer, const auto& data, auto channel)
	{
		ENetPacket* packet = nullptr;
		//packet = create_string_packet(data);
		packet = enet_packet_create(data, sizeof(PositionUpdateMessage), ENET_PACKET_FLAG_RELIABLE);

		enet_peer_send(peer, channel, packet);
	};
}
#pragma endregion Packeting
PositionUpdateMessage deserialize_struct(const auto buffer)
{
	PositionUpdateMessage result;
	memcpy(&result, buffer, sizeof(PositionUpdateMessage));
	return result;
}
Game::Game(int width, int height, bool reliableMessage, bool logCorrections, string correctionsLoggingDirectory)
{
	mapHeight = height;
	mapWidth = width;
	m_client->pos.x = 1;
	m_client->pos.y = 1;
	channel = reliableMessage ? 0 : 1;
	m_client->formConnection();
    countCorrections = logCorrections;

    if (countCorrections) {
        correctionsTracker = new corrections_tracker(correctionsLoggingDirectory, "enet");
    }
}

void Game::startGameLoop() {
	using Framerate = duration<steady_clock::rep, std::ratio<1, 20>>; // 20 ticks per second
	auto next = steady_clock::now() + Framerate{ 1 };

	while (true)
	{
		tick();
		// Sleep until next tick
		std::this_thread::sleep_until(next);
		next += Framerate{ 1 };
	}
	
	enet_peer_disconnect(m_client->peer, 0);

}

void Game::moveLocalPlayer(PositionUpdateMessage newPos)
{
	m_client->pos = newPos;
	//printf("moved local player to x: %d, y: %d\n", m_client->pos.x, m_client->pos.y);
}

// tick performs a single game loop tick, performing the following actions:
// 1. Choosing an action to perform
// 2. Performing this action
// 3. Passing performed action to the networking library
// 4. Process all received messages from networking library
// returns non-zero if an error has occurred
int Game::tick()
{
	
	std::vector<std::pair<PossibleMovements, PositionUpdateMessage>> possibleActions;

	PositionUpdateMessage playerPos = m_client->pos;
	if (playerPos.y >= 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_UP, PositionUpdateMessage(playerPos.x, playerPos.y - 1,0));
	}
	if (playerPos.y <= mapHeight - 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_DOWN, PositionUpdateMessage(playerPos.x, playerPos.y + 1, 0));
	}
	if (playerPos.x >= 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_LEFT, PositionUpdateMessage(playerPos.x - 1, playerPos.y, 0));
	}
	if (playerPos.x <= mapWidth - 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_RIGHT, PositionUpdateMessage(playerPos.x + 1, playerPos.y, 0));
	}

	// Choose a random action to perform
	std::pair<PossibleMovements, PositionUpdateMessage> chosenAction = possibleActions[rand() % (possibleActions.size() > 0 ? possibleActions.size() : 1)];

	// Perform the action
	moveLocalPlayer(chosenAction.second);
	
	m_client->pos.messageType = Event::POSITION_UPDATE;
	uint8_t buffer[sizeof(PositionUpdateMessage)];
	memcpy(buffer, &m_client->pos, sizeof(PositionUpdateMessage));
	Packet::send_packet(m_client->peer, buffer, channel);
	auto event = m_client->event;
	while (enet_host_service(m_client->client, &event, 0) > 0)
	{
		switch (event.type)
		{

		case ENET_EVENT_TYPE_RECEIVE: {

			auto data = deserialize_struct(event.packet->data);

			switch ((Event)data.messageType)
			{
			case Event::PLAYER_JOIN:
			{
				printf("A new client connected from %x:%u.\n", event.peer->address.host, data.ownerId);
				break;
			}
			case Event::POSITION_UPDATE:
                auto prevPosition = m_others[data.ownerId];

				if (countCorrections)
				{
                    correctionsTracker->updatesReceivedInTick();

					if (std::abs(prevPosition.x - data.x) >= MAX_POSITION_CORRECTION || std::abs(prevPosition.y - data.y) >= MAX_POSITION_CORRECTION)
					{
                        correctionsTracker->correctionMade();
					}
				}

                m_others[data.ownerId] = data;
			}

			break;
		}
		case ENET_EVENT_TYPE_DISCONNECT:
			puts("Disconnected retrying...");
			m_client->formConnection();
			break;
		default:
			break;
		}
	}

    if (countCorrections) correctionsTracker->writeLine();

	return 0;
}

