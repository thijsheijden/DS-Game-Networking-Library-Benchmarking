#include "raknet.h"

void NetworkingLibraryRaknet::startReceivingUpdates() {
    while (true) {
        if (!(sendQueue->empty())) {
            auto updateToSend = sendQueue->front();
            printf("NETWORK_LIB: received message to send of type: %u\n", updateToSend.first);

            // Remove this message as it has been processed
            sendQueue->pop();
        }
    }
}