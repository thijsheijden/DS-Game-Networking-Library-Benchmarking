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
Game::Game(int width, int height, bool reliableMessage, bool correction)
{
	mapHeight = height;
	mapWidth = width;
	m_client->pos.x = 1;
	m_client->pos.y = 1;
	channel = reliableMessage ? 0 : 1;
	m_client->formConnection();
}

void Game::startGameLoop() {
	using Framerate = duration<steady_clock::rep, std::ratio<1, 20>>; // 20 ticks per second
	auto next = steady_clock::now() + Framerate{ 1 };
	std::filesystem::path currentPath = std::filesystem::current_path();

	// Specify the subdirectory name
	std::string subdirectoryName = "enet_third_experiment";

	// Combine the current path with the subdirectory name
	std::filesystem::path directoryPath = currentPath / subdirectoryName;

	// Check if the directory already exists
	if (!std::filesystem::exists(directoryPath)) {
		if (std::filesystem::create_directory(directoryPath)) {
			std::cout << "Directory created successfully: " << directoryPath << std::endl;
		}
		else {
			std::cerr << "Error creating directory: " << directoryPath << std::endl;
		}
	}
	else {
		std::cout << "Directory already exists: " << directoryPath << std::endl;
	}

	
	auto currentTime = std::chrono::system_clock::now();
	auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime.time_since_epoch()).count();

	//Total number of corrections made since start(cumulative value)
	//The number of ticks the client has not received any updates from the server
	// (this is incremented every tick the client receives no updates from the server, 
	// and reset back to 0 in the tick the client does receive a server update), this is interesting as it shows how stale the game is
	
	static int fileCount = 0;
	static std::string fileName = "ThirdExperimentEnetClient" + std::to_string(timestamp) + ".csv";
	std::filesystem::path filePath = directoryPath / fileName;
	thirdExperiment.open(filePath);
	if (thirdExperiment) {
		thirdExperiment << " Corrections In Each Tick, No Updates,  Total Corrections,\n ";
	}
	
	
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
	static int correctionCountInTick;
	static int noUpdates = 0;
	static bool isUpdated = false;
	auto event = m_client->event;
	auto dummy = ",";
	while (enet_host_service(m_client->client, &event, 0) > 0)
	{
		isUpdated = false;
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

				m_others[data.ownerId] = data;
				static int prevX;
				static int prevY;
				static bool isTarget = data.ownerId == 0;
				//printf("Recieved position for %d, x: %d,y: %d\n", data.ownerId, data.x, data.y);
				//Number of corrections made based on server updates in the tick(position changes of more than 1 space in any direction)
				if (correctionCheck)
				{
					isUpdated = true;
					
					if (isTarget && (std::abs(prevX - data.x) >= MAX_POSITION_CORRECTION || std::abs(data.y - prevY) >= MAX_POSITION_CORRECTION))
					{
						correctionCountInTick++;
						totalCorrectionCount++;
						
					}
					prevX = data.x;
					prevY = data.y;
				}
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
		if (correctionCheck) 
		{
			if (!isUpdated)
				noUpdates++;
			auto result = std::to_string(correctionCountInTick) + dummy + std::to_string(noUpdates) + dummy+  std::to_string(totalCorrectionCount);
			thirdExperiment << result << std::endl;
		}
	}
	if (correctionCheck)
	{
		correctionCountInTick = 0;
		//noUpdates = 0;
	}
	return 0;
}

