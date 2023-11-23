#include <queue>
#include "../game_events.h"

#ifndef CLIENT_NETWORKING_LIBRARY_H
#define CLIENT_NETWORKING_LIBRARY_H

typedef std::queue<std::pair<EventType, std::variant<PlayerMoveEvent>>> queueType;

// NetworkingLibrary is the abstract base class for all networking library implementations
class NetworkingLibrary
{
public:
    // The queue that the library passes its received updates to, for the local raknet_client to process
    queueType *receivedUpdateQueue;

    // The queue that the local game passes its updates to, for the library to transmit
    queueType *sendQueue;

    NetworkingLibrary(queueType *recvQueue, queueType *sndQueue) : receivedUpdateQueue(recvQueue), sendQueue(sndQueue) {}
    virtual void startReceivingUpdates() = 0;
};

#endif //CLIENT_NETWORKING_LIBRARY_H
