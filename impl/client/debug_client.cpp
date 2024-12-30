#include "debug_client.h"
#include "impl/common/logger.h"
#include "impl/common/memory.h"
#include "impl/common/packets.h"
#include <chrono>
#include <string>
#include <thread>

DebugClient::DebugClient(SocketPort port)
: serverPort(port) {
	this->client = new PacketPollerSocket(8087, false);
	this->initializePacketHandlers();
}

const void DebugClient::connectToServer() {
	sockaddr_in addr { };
	addr.sin_addr = this->client->socketInfo.sin_addr;
	addr.sin_family = this->client->socketInfo.sin_family;
	addr.sin_port = htons(this->serverPort);

	int res = connect(client->socketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in));
	if (res == -1) {
		throw std::logic_error("Failed to connect socket to port: " + std::to_string(client->socketInfo.sin_port));
	}

	this->client->manualInit();
	Logger::getLogger()->info("Sending 'hello' packet!");
	this->client->sendData(
		Packet::wrap(
			C2S::GetBaseAddressPacketType,
			C2S::GetBaseAddressPacketType
		).serialize()
	);
}

void DebugClient::initializePacketHandlers() {
	client->onPacket(S2C::GetBaseAddressPacketType, [this](const Packet packet) {
		auto response = packet.deserialize<S2C::GetBaseAddressPayload>();
		Logger::getLogger()->info("Process responded to 'hello' packet!");
		Logger::getLogger()->info("Base Address is " + MemoryReader::intToHex(response.base_address));

		Logger::getLogger()->info("Tryna request for a read of Base Address...");
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));

		client->sendData(
			Packet::wrap<C2S::ReadMemPayload>(
				C2S::ReadMemPacketType,
				{ .mem_address = response.base_address }
			).serialize()
		);
	});

	client->onPacket(S2C::ReadMemPacketType, [](const Packet packet) {
		Logger::getLogger()->info("Recieved Base Address read!");
		auto response = packet.deserialize<S2C::ReadMemPayload>();
		for (auto val : packet.data) printf("\\x%.2x", val);


		int size = response.data.data_array_size;
		Logger::getLogger()->info("Pakcet size shpuld be: 4 + (x * [4 to size_t]) got " + std::to_string(packet.header.size));
		Logger::getLogger()->info("Base address request response size: " + std::to_string(size));
		if (size > 0) {
			Logger::getLogger()->info("Base address is non-null! Convert to std::vector and get the value!");
			
			bytearray data = net_bytearray::toVector(response.data);
			for (auto val : data) printf("\\x%.2x", val);

			Logger::getLogger()->info("BA -> " + std::to_string(data.at(0)));
		} else {
			Logger::getLogger()->error("Base address request returned null!");
		}
	});
}
