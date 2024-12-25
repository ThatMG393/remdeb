#pragma once

#include "socket/socket.h"
#include <cstddef>
#include <cstdint>
#include <sys/types.h>
#include <vector>

typedef std::vector<u_int8_t> bytearray;

class DebugServer {
public:
    DebugServer(uint16_t port = 8088);

    const void start();
    const void stop();

private:
    SocketServer server;

    bytearray readMemory(u_int32_t address);
    void writeMemory(u_int32_t address, bytearray data);

    void initializePacketHandlers();
};
