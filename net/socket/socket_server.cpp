#include "socket_server.h"
#include "impl/common/logger.h"
#include <algorithm>
#include <memory>
#include <stdexcept>
#include <string>
#include <thread>

SocketServer::SocketServer(ServerConfig config) {
	serverSocket = std::make_unique<Socket>(config.port);
	this->init();
}

SocketServer::~SocketServer() {	
	serverSocket.reset();
	acceptorThread.reset();
}

int SocketServer::on(PacketType type, std::function<void(const Client2ServerPacket)> handler) {
	Logger::getLogger().info("Registered new handler for Packet with type: " + std::to_string(type));

	auto subscribers = handlers[type];
	int index = subscribers.size();
	subscribers.push_back(handler);
	handlers[type] = subscribers;
	return index;
}

void SocketServer::removeOn(PacketType type, int index) {
	auto subscribers = handlers[type];
	if (subscribers.size() == 0) return;

	subscribers.erase(subscribers.begin() + index);
}

void SocketServer::start() {
	if (running.load()) return;
	Logger::getLogger().info("SocketServer started at port: " + std::to_string(htons(serverSocket->socketInfo.sin_port)));

	running.store(true);
	if (acceptorThread.get() == nullptr) acceptorThread = std::make_unique<std::thread>(&SocketServer::acceptClients, this);
}

void SocketServer::stop() {
	if (!running.load()) return;

	running.store(false);
	if (acceptorThread.get() != nullptr && acceptorThread->joinable()) acceptorThread->join();
	delete this;
}

void SocketServer::broadcast(const Client2ServerPacket packet) {
	for (Client client : clients) client.socket->sendData(packet.data.serialize());
}

void SocketServer::notifyHandlers(const Client2ServerPacket packet) {
	auto subscribers = handlers[packet.data.header.type];

	Logger::getLogger().info("Notifying " + std::to_string(subscribers.size()) + " listeners for packet with type: " + std::to_string(packet.data.header.type));
	for (size_t i = 0; i < subscribers.size(); ++i) subscribers[i](packet);
}

void SocketServer::init() {
	int res = listen(serverSocket->socketFd, 1);
	if (res < 0) {
		throw std::logic_error("Failed to make the SocketServer listen for clients/s!");
	}
}

void SocketServer::acceptClients() {
	Logger::getLogger().info("Will now listen for clients.");
	while (running.load()) {
		sockaddr_in clientAddr = { };
		socklen_t clientAddrLen = sizeof(clientAddr);

		SocketFd clientFd = accept(
			serverSocket->socketFd,
			reinterpret_cast<sockaddr*>(&clientAddr),
			&clientAddrLen
		);

		if (clientFd == INVALID_SOCKET) continue;

		Logger::getLogger().info("Client with Fd " + std::to_string(clientFd) + " connected.");
		std::shared_ptr<PacketPollerSocket> client = std::make_shared<PacketPollerSocket>(clientFd, clientAddr);
		client->onPacket([this, client](const Packet packet) {
			notifyHandlers({
				.sender = { .socket = client },
				.data = packet
			});
		});
		client->onClose([this, client](const int res) {
			Logger::getLogger().info("Client with Fd " + std::to_string(client->socketFd) + " disconnected.");
			clients.erase(
				std::remove_if(clients.begin(), clients.end(),
				[client](const Client& c) {
					return c.socket->socketFd == client->socketFd;
				})
			);
		});
		clients.push_back({ .socket = client });

		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}
}

