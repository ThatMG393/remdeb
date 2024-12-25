#include "debug_server.h"
#include "socket.h"
#include <cstddef>
#include <cstdint>

DebugServer::DebugServer(uint16_t port, size_t maxConnections, bool nonBlocking) {
    this->server = SocketServer({
        .port = port,
        .max_connections = maxConnections,
        .non_blocking = nonBlocking
    });
}

