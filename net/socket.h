#pragma once

#include <atomic>
#include <thread>
#include <vector>
#include <mutex>
#include <functional>
#include <memory>
#include <unordered_map>
#include <queue>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <string.h>

using SocketType = int;
using SockLenType = socklen_t;
constexpr int INVALID_SOCKET = -1;
constexpr int SOCKET_ERROR = -1;

typedef std::vector<u_int8_t> bytearray;

struct PacketHeader {
    uint16_t type;
    uint32_t size;
};

template<typename PayloadType>
class Packet {
public:
    static constexpr uint16_t Type = 0;
    PayloadType payload;

    bytearray serialize() const;
    static Packet<PayloadType> deserialize(const bytearray& data);
};

struct ClientInfo {
    SocketType socket;
    std::string ip;
    uint16_t port;
};

struct ServerConfig {
    uint16_t port = 8080;
    size_t max_connections = 100;
    bool non_blocking = true;
};

template<typename T>
class ThreadSafeQueue {
    std::queue<T> queue;
    std::mutex mutex;

public:
    void push(T item);
    bool pop(T& item);
};

class SocketServer {
private:
    SocketType server_socket = INVALID_SOCKET;
    ServerConfig config;
    std::atomic<bool> running{false};

    std::vector<ClientInfo> clients;
    std::mutex clients_mutex;

    std::unordered_map<uint16_t, std::function<void(const bytearray&)>> handlers;
    ThreadSafeQueue<std::pair<ClientInfo, bytearray>> packet_queue;

    std::unique_ptr<std::thread> accept_thread;
    std::unique_ptr<std::thread> process_thread;

    void initialize_socket();
    void accept_connections();
    void process_packets();

public:
    SocketServer(const ServerConfig& cfg = {});

	template<typename PacketType>
	void on(std::function<void(const PacketType&)> handler) {
    	handlers[PacketType::Type] = [handler](const bytearray& data) {
        	auto packet = PacketType::deserialize(data);
        	handler(packet);
    	};
	}

    void start();
    void stop();
    ~SocketServer();
};

