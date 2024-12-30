#pragma once

#include "packet_poller_socket.h"
#include <atomic>
#include <memory>
#include <thread>

struct Client {
	std::shared_ptr<PacketPollerSocket> socket;
};

struct Client2ServerPacket {
	Client sender;
	Packet data;
};

struct Server2ClientPacket {
	Client target;
	Packet data;
};

struct ServerConfig {
	SocketPort port;
};

class SocketServer {
public:
	SocketServer(ServerConfig);
	~SocketServer();

	int on(PacketType, std::function<void(const Client2ServerPacket)>);
	void removeOn(PacketType, int);

	void start();
	void stop();

	void broadcast(Client2ServerPacket);

	bool isRunning() const { return running.load(); }

protected:
	void notifyHandlers(const Client2ServerPacket);

private:
	std::unique_ptr<Socket> serverSocket;
	std::unique_ptr<std::thread> acceptorThread;
	std::unordered_map<PacketType, std::vector<std::function<void(const Client2ServerPacket)>>> handlers;
	std::vector<Client> clients;
	std::atomic_bool running {false};

	void init();
	void acceptClients();
};
