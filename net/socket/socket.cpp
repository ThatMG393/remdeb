#include "socket.h"
#include "impl/common/logger.h"
#include "net/packet/packet.h"
#include <linux/in.h>
#include <stdexcept>
#include <string>
#include <sys/endian.h>

Socket::Socket(SocketFd fd, sockaddr_in info, bool autoInit)
	: socketFd(fd), socketInfo(info) {
	if (autoInit) this->init();
}

Socket::~Socket() {
	if (socketFd > 0) {
		shutdown(socketFd, SHUT_RDWR);
		close(socketFd);

		Logger::getLogger().info("Successfully disposed socket.");
	}
}

Socket::Socket(SocketPort port) 
    : Socket(socket(AF_INET, SOCK_STREAM, 0), [&]() {
		sockaddr_in addr { };
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        return addr;
    }()) { }

int Socket::sendData(bytearray data) {
	int res = send(socketFd, data.data(), data.size(), 0);
	if (res == -1) {
		Logger::getLogger().warn("Failed to send " + std::to_string(data.size()) + " bytes to SocketFd " + std::to_string(socketFd));
	}

	return res;
}

int Socket::recieveData(bytearray* buffer, bool waitUntilFullBuf) {
	if (buffer->size() <= 0) return -1;

	int res = recv(socketFd, buffer->data(), buffer->size(), waitUntilFullBuf ? MSG_WAITALL : 0);
	if (res == -1) {
		Logger::getLogger().warn("Failed to recieve " + std::to_string(buffer->size()) + " bytes from SocketFd " + std::to_string(socketFd));
	}

	return res;
}

void Socket::verify() {
	if (socketFd == INVALID_SOCKET) {
		throw std::logic_error("Failed to create a socket! Error: " + getErrorString());
	}
}

void Socket::init() {
	this->verify();

	const int result = 1;
	setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &result, sizeof(int));

	int res = bind(socketFd, reinterpret_cast<sockaddr*>(const_cast<sockaddr_in*>(&socketInfo)), sizeof(socketInfo));
	if (res == -1) {
        throw std::logic_error("Failed to bind socket to port: " + 
            std::to_string(socketInfo.sin_port) + 
            " Error: " + getErrorString());
	}
}

