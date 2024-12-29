#include "net/packet/packet.h"
#include "socket.h"
#include <functional>
#include <thread>
#include <vector>

class PacketPollerSocket : public Socket {
public:
	PacketPollerSocket(SocketFd, sockaddr_in);

	void onPacket(std::function<void(const Packet)>);
	void onClose(std::function<void(const int)>);

protected:
	void init() override;

private:
	std::vector<std::function<void(Packet)>> onPacketCallbacks;
	std::vector<std::function<void(int)>> onCloseCallbacks;

	std::unique_ptr<std::thread> pollingPacketsThread;

	void pollPackets();
	void callPacketCallbacks(Packet);
	void callCloseCallbacks(int);
};

