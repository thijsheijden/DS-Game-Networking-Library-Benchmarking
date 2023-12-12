#pragma once

#include "slikenet/MessageIdentifiers.h"

u_char GetPacketIdentifier(SLNet::Packet *p) {
    if ((unsigned char)p->data[0] == ID_TIMESTAMP)
        return (u_char) p->data[sizeof(SLNet::MessageID) + sizeof(SLNet::Time)];
    else
        return (u_char) p->data[0];
};
