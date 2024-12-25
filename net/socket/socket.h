#pragma once

#include "packet/packet.h"
#include <netinet/in.h>
#include <atomic>
#include <functional>
#include <thread>
#include <vector>

typedef std::vector<u_int8_t> bytearray;
typedef u_int16_t PacketType;
typedef int SocketFd;

const SocketFd INVALID_SOCKET = -1;
const SocketFd ERRORED_SOCKET = -1;

struct ClientInfo {
	SocketFd clientFd;
	sockaddr_in clientAddress;
};

struct SocketConfig {
	u_int16_t port;
};

struct PacketHeaderExt {
	PacketHeader header;
	ClientInfo sender;
};

class SocketServer;
class ClientSocket {
public:
	const ClientInfo info;
	
	SocketServer* parent;
	std::thread processThread;

	ClientSocket(const ClientInfo, SocketServer* parent);

private:
	void processData();
};

class SocketServer {
public:
	SocketServer(const SocketConfig);

	void start();
	void stop();

	template<typename PacketType, typename Payload>
	void on(std::function<void(const Payload)> handler) {
		handlers[PacketType::Type].push_back(
			[handler](const bytearray& data) {
				handler(Packet<Payload>::deserialize(data));
			}
		);
	}

	bool isRunning() const {
		return running;
	}

	void _internal_notifyHandlers(PacketHeaderExt header, bytearray data);
	
private:
	const SocketConfig config;

	std::atomic<bool> running{false};
	std::vector<ClientSocket> clients;
	std::unordered_map<PacketType, std::vector<std::function<void(const bytearray&)>>> handlers;

	std::thread serverThread;

	SocketFd serverFd;

	void init();
	void handleIncoming();
	void processData();
};
