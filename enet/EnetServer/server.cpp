#include <iostream>
#include "enet/enet.h"
#include "common.h"
#include <unordered_map>
#include <thread>
#include <future>
#include <fstream>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")
#define MAX_POSITION_CORRECTION  2
#define BENCHMARK 

struct ClientData {
	uint32_t id;
	uint32_t score;

};
namespace Packet
{
	ENetPacket* create_string_packet(const std::string& data)
	{
		return  enet_packet_create(data.c_str(), data.size() + 1, ENET_PACKET_FLAG_RELIABLE);
	}

}
PositionUpdateMessage deserialize_struct(auto buffer)
{
	PositionUpdateMessage result;
	memcpy(&result, buffer, sizeof(PositionUpdateMessage));
	return result;
}

void introduceLatency(auto& event, auto& prevPos, auto& pos)
{
	static std::mutex posMutex;
	std::thread latency([&event, &prevPos, &pos]() {
		{std::lock_guard<std::mutex> lock(posMutex);
		prevPos = pos; }
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		});
	latency.detach();
}
int main(int argc, char** argv)
{
	if (enet_initialize() != 0)
	{
		fprintf(stderr, "An error occurred while initializing ENet.\n");
		return EXIT_FAILURE;
	}
	atexit(enet_deinitialize);
	ENetAddress address;
	ENetHost* server;
	ENetEvent event;
	int fixCount = -1;

	address.host = ENET_HOST_ANY;
	address.port = 60000;

	server = enet_host_create(&address, 32, 2,
		0      /* assume any amount of incoming bandwidth */,
		0      /* assume any amount of outgoing bandwidth */);

	if (server == NULL)
	{
		fprintf(stderr,
			"An error occurred while trying to create an ENet server host.\n");
		exit(EXIT_FAILURE);
	}
	ENetPacket* packet;
	PositionUpdateMessage pos;
	PositionUpdateMessage prevPos;
	PositionUpdateMessage val;
	ENetPeer* sender;
	auto simulationDuration = std::chrono::seconds(120);
	auto startTime = std::chrono::high_resolution_clock::now();
	ENetPeer* peer;
	int clientMessageCount = 0;
#pragma region game_loop
	while (true)
	{
		while (enet_host_service(server, &event, 100) > 0)
		{
			auto now = std::chrono::high_resolution_clock::now();
			if (now - startTime >= simulationDuration)
			{
				break;
			}
			switch (event.type)
			{
				//Client connect 
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);
				PositionUpdateMessage pos(0, 0, event.peer->incomingPeerID);
				NewPlayerJoinMessage newPlayer(pos);
				uint8_t buffer[sizeof(NewPlayerJoinMessage)];
				memcpy(buffer, &pos, sizeof(NewPlayerJoinMessage));
				packet = enet_packet_create(buffer, sizeof(NewPlayerJoinMessage), ENET_PACKET_FLAG_RELIABLE);
				enet_host_broadcast(server, 0, packet);
				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				val = deserialize_struct(event.packet->data);

				switch ((Event)val.messageType)
				{
					//Broadcast positon update
				case Event::POSITION_UPDATE:

					sender = event.peer;
					pos.messageType = Event::POSITION_UPDATE;
					pos.ownerId = event.peer->incomingPeerID;
					pos.x = val.x;
					pos.y = val.y;
					//does not actually corrects the position
					static int count = 0;

#ifdef BENCHMARK

					if (event.peer->incomingPeerID == 2)
					{
						static bool tOnce = 1;
						static bool tLatencyAdded = false;
						if (tOnce)
						{
							printf("id 2 port: %d", event.peer->address.port);
							tOnce = 0;
						}
						/*This is normally where the correction logic (e.g std::abs(pos.x - prevPos.x) > MAX_POSITION_CORRECTION || std::abs(pos.y - prevPos.y) > MAX_POSITION_CORRECTION) happens.
							Points to take into consideration :
							1. Enet polls messages when latency is introduced in the network, then process polled data consecutively.
							2. Above feature will create rubber banding if it wont handled correctly.
							3. As it is known that Enet
									a. aims reliable messaging
									b. polls data until destination reached and all data processed consecutively when reached
							the correctness mechanism should have worked at the moment when position updated more than once for same client consecutively.
						*/
						if (count >= 1)
						{
							fixCount++;
							tLatencyAdded = true;
						}
						if (tLatencyAdded)
						{
							clientMessageCount++;
						}
						std::printf("count %d", count++);
					}
					else
					{
						count = 0;
					}
#endif // BENCHMARK
					uint8_t buffer[sizeof(PositionUpdateMessage)];
					memcpy(buffer, &pos, sizeof(PositionUpdateMessage));
					packet = enet_packet_create(buffer, sizeof(PositionUpdateMessage), ENET_PACKET_FLAG_RELIABLE);
					for (size_t i = 0; i < server->peerCount; ++i) {
						peer = &server->peers[i];
						if (peer != sender) {
							enet_peer_send(peer, 0, packet);
						}
					}
					break;
				}
				//Clean up the packet

				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("%s disconnected.\n", event.peer->data);
				break;
			}

			}
		}
		auto now = std::chrono::high_resolution_clock::now();
		if (now - startTime >= simulationDuration)
		{
			break;
		}


	}
#pragma endregion game_loop
	std::ofstream file("fix.txt");
	file << fixCount << " out of total" << clientMessageCount;
	file.close();
	enet_host_destroy(server);
	return EXIT_SUCCESS;

}