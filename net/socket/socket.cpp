#include "socket.h"
#include <cstring>
#include <sys/endian.h>
#include <sys/socket.h>
#include <thread>


ClientSocket::ClientSocket(const ClientInfo info, SocketServer* parent)
 : info(info), parent(parent) {
 	 this->processThread = std::thread(&ClientSocket::processData, this);
}

void ClientSocket::processData() {
	while (this->parent->isRunning()) {
		bytearray headerBuf(sizeof(PacketHeader));
		recv(this->info.clientFd, headerBuf.data(), headerBuf.size(), 0);

		if (headerBuf.size() != sizeof(PacketHeader)) continue;

		PacketHeader header;
		std::memcpy(&header, headerBuf.data(), sizeof(PacketHeader));

		bytearray packetBuf(header.size);
		recv(this->info.clientFd, packetBuf.data(), packetBuf.size(), 0);

		this->parent->_internal_notifyHandlers({ .header = header, .sender = this->info }, packetBuf);
	}
}

SocketServer::SocketServer(const SocketConfig config)
 : config(config) {
	this->init();
}

void SocketServer::start() {
	if (this->running) return;

	this->serverThread = std::thread(&SocketServer::handleIncoming, this);	
	this->running = true;
}

void SocketServer::stop() {
	if (!this->running) return;
	this->running = false;

	if (this->serverThread.joinable()) this->serverThread.join();
}

void SocketServer::init() {
	this->serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if (this->serverFd == INVALID_SOCKET) return;

	sockaddr_in address = {};
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(this->config.port);
	address.sin_family = AF_INET;

	auto _ = bind(this->serverFd, reinterpret_cast<sockaddr*>(&address), sizeof(address));
	listen(this->serverFd, 1);
}

void SocketServer::handleIncoming() {
	while (this->running) {
		sockaddr_in clientAddr = {};
		socklen_t clientAddrLen = sizeof(clientAddr);

		SocketFd client = accept(
			this->serverFd,
			reinterpret_cast<sockaddr*>(&clientAddr),
			&clientAddrLen
		);

		if (client == INVALID_SOCKET) continue;

        this->clients.push_back(ClientSocket({
            .clientFd = client,
            .clientAddress = clientAddr
        }, this));

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void SocketServer::_internal_notifyHandlers(PacketHeaderExt header, bytearray data) {
	auto subscribers = this->handlers[header.header.type];
	for (auto subscriber : subscribers) {
		subscriber(data);
	}
}
