#pragma once

#include "net/packet/packet.h"
#include <atomic>
#include <cstring>
#include <functional>
#include <memory>
#include <netinet/in.h>
#include <optional>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unordered_map>
#include <vector>

typedef u_int16_t SocketPort;
typedef u_int32_t DataLength;
typedef int SocketFd;

static SocketFd INVALID_SOCKET = -1;

class Socket {
public:
	Socket(SocketPort);
	Socket(SocketFd, sockaddr_in, bool = true);
	virtual ~Socket();

	const SocketFd socketFd;
	const sockaddr_in socketInfo;

	int sendData(bytearray);
	int recieveData(bytearray*, bool = true);

protected:
	void verify();
	virtual void init();
};

static std::string getErrorString() {
	return std::string(strerror(errno));
}
