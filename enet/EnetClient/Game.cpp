#include "Game.h"
#include "chrono"
#include "thread"
using namespace std::chrono;

#pragma region Packeting
namespace Packet
{
	ENetPacket* create_string_packet(const std::string& data)
	{
		return  enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
	}

	decltype(auto) send_packet = [](ENetPeer* peer, const auto& data)
	{
		ENetPacket* packet = nullptr;
		packet = create_string_packet(data);
		enet_peer_send(peer, 0, packet);
	};
}
#pragma endregion Packeting

Game::Game(int width, int height)
{
	mapHeight = height;
	mapWidth = width;
	m_client->pos.x = 1;
	m_client->pos.y = 1;
	m_client->formConnection();
}

void Game::startGameLoop() {
	using Framerate = duration<steady_clock::rep, std::ratio<1, 5>>; // 5 ticks per second
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

void Game::moveLocalPlayer(Position newPos)
{
	m_client->pos = newPos;
	printf("moved local player to x: %d, y: %d\n", m_client->pos.x, m_client->pos.y);
}

// tick performs a single game loop tick, performing the following actions:
// 1. Choosing an action to perform
// 2. Performing this action
// 3. Passing performed action to the networking library
// 4. Process all received messages from networking library
// returns non-zero if an error has occurred
int Game::tick()
{
	std::vector<std::pair<PossibleMovements, Position>> possibleActions;

	Position playerPos = m_client->pos;
	if (playerPos.y >= 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_UP, Position(playerPos.x, playerPos.y - 1));
	}
	if (playerPos.y <= mapHeight - 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_DOWN, Position(playerPos.x, playerPos.y + 1));
	}
	if (playerPos.x >= 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_LEFT, Position(playerPos.x - 1, playerPos.y));
	}
	if (playerPos.x <= mapWidth - 1) {
		possibleActions.emplace_back(PossibleMovements::PLAYER_MOVE_RIGHT, Position(playerPos.x + 1, playerPos.y));
	}

	// Choose a random action to perform
	std::pair<PossibleMovements, Position> chosenAction = possibleActions[rand() % (possibleActions.size() > 0 ? possibleActions.size() : 1)];

	// Perform the action
	moveLocalPlayer(chosenAction.second);
	m_client->pos.opt_code = 5;
	std::string serialized = m_client->pos.serialize(m_client->peer->outgoingPeerID);

	Packet::send_packet(m_client->peer, serialized);

	auto event = m_client->event;
	while (enet_host_service(m_client->client, &event, 0) > 0)
	{
		switch (event.type)
		{
		case ENET_EVENT_TYPE_RECEIVE: {

			int eventType = 0;
			std::string convertedString(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
			Position pos;
			pos.deserialize(convertedString, eventType);
			pos.opt_code = eventType;
			switch ((Event)eventType)
			{

			case Event::RECIEVED_POSITION:

				m_others[pos.ownerId] = pos;
				printf("Recieved position for %d, x: %d,y: %d\n", pos.ownerId, pos.x, pos.y);
				break;
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
	return 0;
}
//TODO: Implement if neccessary
void Gamestate::updateGameState(std::vector<Client> pClients, std::vector<Bomb>)
{
	otherPlayers = pClients;
}
