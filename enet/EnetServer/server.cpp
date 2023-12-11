#include <iostream>
#include "Packet.hpp"
#include "Events.hpp"
#include "common.h"
#include <unordered_map>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "winmm.lib")

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
static std::unordered_map<uint32_t, ClientData*> g_clients;

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
	uint32_t clientId = 0;

	Network::Packet packet;
	Network::Packet map_size_packet;
	Position pos;
	bool lostOnce =0;
#pragma region game_loop
	while (true)
	{
		while (enet_host_service(server, &event, 100) > 0)
		{
			switch (event.type)
			{
			case ENET_EVENT_TYPE_CONNECT:
			{
				printf("A new client connected from %x:%u.\n",
					event.peer->address.host,
					event.peer->address.port);

				ClientData cl;
				cl.id = clientId;
				cl.score = 0;
				if (g_clients.count(clientId) == 0)
				{
					g_clients[clientId] = new ClientData(cl);
				}
				clientId++;
				packet << static_cast<uint8_t>(Events::CLIENT_CONNECTED) << clientId;
				Network::send_packet(event.peer, packet, 0, ENET_PACKET_FLAG_RELIABLE);

				auto data = pos.serialize(event.peer->incomingPeerID);
				auto dist = Packet::create_string_packet(data);
				//broadcast to client
				enet_host_broadcast(server, 0, dist);
				if (clientId == 2)
				{
					uint32_t restrictBandwidth = 5; // 0.00004 Mbps
					event.peer->outgoingBandwidth = restrictBandwidth;
				}

				break;
			}
			case ENET_EVENT_TYPE_RECEIVE:
			{
				//use port 1 than 0

				int eventType = 0;
				std::string convertedString(reinterpret_cast<char*>(event.packet->data), event.packet->dataLength);
				pos.deserialize(convertedString, eventType);
				switch ((Event)eventType)
				{

				case Event::BROADCAST_POSITION:
					auto sender = event.peer;
					pos.opt_code = 6;
					auto data = pos.serialize(event.peer->incomingPeerID);
					auto dist = Packet::create_string_packet(data);
					for (size_t i = 0; i < server->peerCount; ++i) {
						ENetPeer* peer = &server->peers[i];
						if (peer != sender) {
							// Send the packet to the peer
							enet_peer_send(peer, 0, dist);
						}
					}
					printf("Broadcasted position for %d, x: %d,y: %d \n", event.peer->incomingPeerID, pos.x, pos.y);
					if (event.peer->incomingPeerID == 2 && event.peer->packetsLost > 0)
					{
						std::cout << "Client " << event.peer->incomingPeerID << ": Sent " << event.peer->packetsSent
							<< " packets, Dropped " << event.peer->packetsLost << " packets." << "data size :" << event.packet->dataLength <<std::endl;
						lostOnce = 1;
					}
					else if(lostOnce && event.peer->incomingPeerID == 2 && event.peer->packetsLost <= 0)
					{
						printf("I RECOVERED! \n");
						lostOnce = 0;
					}
					break;
				}
				/* Clean up the packet now that we're done using it. */
				enet_packet_destroy(event.packet);

				break;
			}
			case ENET_EVENT_TYPE_DISCONNECT:
			{
				printf("%s disconnected.\n", event.peer->data);

				/* Reset the peer's client information. */

				ClientData* client = reinterpret_cast<ClientData*>(&event.peer->data);

				if (client != nullptr)
				{
					g_clients.erase(client->id);
					Network::Packet packet;
					packet << static_cast<uint8_t>(Events::CLIENT_DISCONNECTED) << client->id;
					enet_host_broadcast(server, 0, Network::create_packet(packet, ENET_PACKET_FLAG_RELIABLE));
					event.peer->data = nullptr;
				}
				break;
			}

			}
		}
	}

#pragma endregion game_loop
	for (auto& client : g_clients) {
		delete client.second;
	}
	enet_host_destroy(server);
	return EXIT_SUCCESS;

}