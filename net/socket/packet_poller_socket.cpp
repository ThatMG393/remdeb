#include "packet_poller_socket.h"
#include "common/logger.h"
#include "packet/packet.h"

PacketPollerSocket::PacketPollerSocket(SocketPort port, bool autoInit)
	: Socket(port, autoInit) {
	if (autoInit) this->init();
}

PacketPollerSocket::PacketPollerSocket(
	SocketFd fd, sockaddr_in info, bool autoInit
) : Socket(fd, info, false) {
	if (autoInit) this->init();
}

void PacketPollerSocket::init() {
	if (ranInit.load()) return;
	ranInit.store(true);

	Socket::verify();
	if (pollingPacketsThread.get() == nullptr) pollingPacketsThread = std::make_unique<std::thread>(&PacketPollerSocket::pollPackets, this);
}

void PacketPollerSocket::pollPackets() {
	if (!ranInit.load()) {
		Logger::getLogger()->error("Will not poll packets as PacketPollerSocket::init() hasn't been called yet!");
		return;
	}
	
	Logger::getLogger()->info("Will now poll packets from SocketFd " + std::to_string(socketFd));

	int res;
	while (true) {
		bytearray headerBuf(sizeof(PacketHeader));
		res = recieveData(&headerBuf);
		if (res == 0) break;
		if (res == -1) continue;

		PacketHeader header;
		std::memcpy(&header, headerBuf.data(), sizeof(PacketHeader));

		Logger::getLogger()->info("Recieved Packet from SocketFd " + std::to_string(socketFd));

		bytearray dataBuf(header.size);
		res = recieveData(&dataBuf);
		if (res == 0) break;
		if (res == -1) continue;

		Packet packet(header, dataBuf);
		callAnyPacketCallbacks(packet);
		callPacketCallbacks(packet);
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));		
	}

	callCloseCallbacks(res);
}

void PacketPollerSocket::onPacket(PacketType type, std::function<void(Packet)> callback) {
	this->onPacketCallbacks[type] = callback;
}

void PacketPollerSocket::onAnyPacket(std::function<void(Packet)> callback) {
	this->onAnyPacketCallbacks.push_back(callback);
}

void PacketPollerSocket::onClose(std::function<void(int)> callback) {
	this->onCloseCallbacks.push_back(callback);
}

void PacketPollerSocket::callPacketCallbacks(Packet packet) {
	if (onPacketCallbacks.size() == 0) return;
	this->onPacketCallbacks[packet.header.type](packet);
}

void PacketPollerSocket::callAnyPacketCallbacks(Packet packet) {
	if (onAnyPacketCallbacks.size() == 0) return;
	for (const auto& callback : onAnyPacketCallbacks) callback(packet);
}

void PacketPollerSocket::callCloseCallbacks(int errorCode) {
	if (onCloseCallbacks.size() == 0) return;
	for (const auto& callback : onCloseCallbacks) callback(errorCode);
}
