#pragma once

#include "net/socket/socket_server.h"

class DebugServer {
public:
    DebugServer(uint16_t = 8088);

    const void start();
    const void stop();

private:
    SocketServer server;
 
    void initializePacketHandlers();
};
