#include "debug_server.h"
#include "impl/common/logger.h"
#include "impl/common/packets.h"
#include "impl/common/memory.h"

DebugServer::DebugServer(uint16_t port)
 : server(SocketServer({ .port = port })) {
	this->initializePacketHandlers();
}

const void DebugServer::start() {
	this->server.start();
}

const void DebugServer::stop() {
	this->server.stop();
}

void DebugServer::initializePacketHandlers() {
	this->server.on(C2S::ReadMemPacketType, [](const Client2ServerPacket packet) {
		Logger::getLogger()->info("ReadMemPacket recieved!");
		C2S::ReadMemPayload payload = packet.data.deserialize<C2S::ReadMemPayload>().value();
		Logger::getLogger()->info("Reading -> " + MemoryReader::intToHex(payload.mem_address));
		
		packet.sender.socket->sendData(
			Packet::wrap<S2C::ReadMemPayload>(
				S2C::ReadMemPacketType,
				{ .data = net_bytearray(MemoryReader::readBytes(payload.mem_address, 1).value_or(bytearray({1, 0, 1}))) }
			).serialize()
		);
	});

	this->server.on(C2S::WriteMemPacketType, [](const Client2ServerPacket packet) {
		Logger::getLogger()->info("WriteMemPacket recieved!");
		auto _ = packet.data.deserialize<C2S::WriteMemPayload>();

		packet.sender.socket->sendData(bytearray());
	});

	stop();
}
