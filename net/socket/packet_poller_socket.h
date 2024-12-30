#pragma once

#include "net/packet/packet.h"
#include "socket.h"
#include <atomic>
#include <functional>
#include <thread>
#include <unordered_map>
#include <vector>

class PacketPollerSocket : public Socket {
public:
	PacketPollerSocket(SocketPort port, bool = true);
	PacketPollerSocket(SocketFd, sockaddr_in, bool = true);

	void onPacket(PacketType, std::function<void(const Packet)>);
	void onAnyPacket(std::function<void(const Packet)>);
	void onClose(std::function<void(const int)>);

	void manualInit() { this->init(); }

protected:
	void init() override;

private:
	std::unordered_map<PacketType, std::function<void(const Packet)>> onPacketCallbacks;
	std::vector<std::function<void(const Packet)>> onAnyPacketCallbacks;
	std::vector<std::function<void(int)>> onCloseCallbacks;

	std::unique_ptr<std::thread> pollingPacketsThread;

	void pollPackets();
	void callPacketCallbacks(Packet);
	void callAnyPacketCallbacks(Packet);
	void callCloseCallbacks(int);

	std::atomic_bool ranInit { false };
};

