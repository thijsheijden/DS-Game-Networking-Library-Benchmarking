#include "slikenet/NetworkIDManager.h"
#import "libnet.h"

#ifndef CLIENT_CLIENT_H
#define CLIENT_CLIENT_H

using namespace SLNet;
class Client
{
public:
    NetworkIDManager manager;

    Client() = default;
};

#endif //CLIENT_CLIENT_H