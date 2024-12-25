#include "debug_server.h"
#include "impl/common/packets.h"
#include "socket/socket.h"
#include <cstdio>

DebugServer::DebugServer(uint16_t port)
 : server(SocketServer({ .port = port })) {
	initializePacketHandlers();
}

const void DebugServer::start() {
	this->server.start();
}

const void DebugServer::stop() {
	this->server.stop();
}

void DebugServer::initializePacketHandlers() {
	this->server.on<ReadMemPacket, ReadMemPayload>([this](const ReadMemPayload& packet) {
		printf("NO WAY READMEMPACK!");
	});
}
