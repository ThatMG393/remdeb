#include "packet_poller_socket.h"
#include "packet/packet.h"

PacketPollerSocket::PacketPollerSocket(
	SocketFd fd, sockaddr_in info
) : Socket(fd, info, false) {
	this->init();
}

void PacketPollerSocket::init() {
	Socket::verify();
	if (pollingPacketsThread.get() == nullptr) pollingPacketsThread = std::make_unique<std::thread>(&PacketPollerSocket::pollPackets, this);
}

void PacketPollerSocket::pollPackets() {
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

		callPacketCallbacks(Packet(header, dataBuf));
		
		std::this_thread::sleep_for(std::chrono::milliseconds(100));		
	}

	callCloseCallbacks(res);
}

void PacketPollerSocket::onPacket(std::function<void(Packet)> callback) {
	this->onPacketCallbacks.push_back(callback);
}

void PacketPollerSocket::onClose(std::function<void(int)> callback) {
	this->onCloseCallbacks.push_back(callback);
}

void PacketPollerSocket::callPacketCallbacks(Packet packet) {
	if (onPacketCallbacks.size() == 0) return;
	for (const auto& callback : onPacketCallbacks) callback(packet);
}

void PacketPollerSocket::callCloseCallbacks(int errorCode) {
	if (onCloseCallbacks.size() == 0) return;
	for (const auto& callback : onCloseCallbacks) callback(errorCode);
}
