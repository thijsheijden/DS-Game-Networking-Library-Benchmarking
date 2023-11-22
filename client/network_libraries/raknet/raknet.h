#include "../networking_library.h"

#ifndef CLIENT_RAKNET_H
#define CLIENT_RAKNET_H

// NetworkingLibraryRaknet is a concrete implementation of the NetworkingLibrary abstract class
class NetworkingLibraryRaknet: public NetworkingLibrary
{
public:
    NetworkingLibraryRaknet(queueType *recvQueue, queueType *sndQueue) : NetworkingLibrary(recvQueue, sndQueue) {};
    void startReceivingUpdates();
};

#endif //CLIENT_RAKNET_H
