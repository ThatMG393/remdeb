#pragma once

#include "net/socket/socket.h"
#include "net/socket/packet_poller_socket.h"

class DebugClient {
public:
    DebugClient(SocketPort = 8088);

    const void connectToServer();

private:
	const SocketPort serverPort;

    PacketPollerSocket* client;
 
    void initializePacketHandlers();
};
