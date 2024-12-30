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
	this->server.on(C2S::GetBaseAddressPacketType, [](const Client2ServerPacket packet) {
		Logger::getLogger()->info("Recieved a 'hello' packet!");
		packet.sender.socket->sendData(
			Packet::wrap<S2C::GetBaseAddressPayload>(
				S2C::GetBaseAddressPacketType,
				{ .base_address = MemoryReader::getProcessBaseAddress() }
			).serialize()
		);
		Logger::getLogger()->info("Successfully replied back!");
	});

	this->server.on(C2S::ReadMemPacketType, [](const Client2ServerPacket packet) {
		Logger::getLogger()->info("ReadMemPacket recieved!");
		C2S::ReadMemPayload payload = packet.data.deserialize<C2S::ReadMemPayload>();

		Logger::getLogger()->info("Reading -> " + MemoryReader::intToHex(payload.mem_address));
		
		packet.sender.socket->sendData(
			Packet::wrap<S2C::ReadMemPayload>(
				S2C::ReadMemPacketType,
				{ .data = net_bytearray(bytearray({255, 69})) }
			).serialize()
		);
		Logger::getLogger()->info("Sent reply!");
	});
}
