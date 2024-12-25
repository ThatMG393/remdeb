#include "socket.h"

template<typename PayloadType>
bytearray Packet<PayloadType>::serialize() const {
    bytearray buffer(sizeof(PacketHeader) + sizeof(PayloadType));
    PacketHeader header{Type, sizeof(PayloadType)};
    
    memcpy(buffer.data(), &header, sizeof(PacketHeader));
    memcpy(buffer.data() + sizeof(PacketHeader), &payload, sizeof(PayloadType));
    
    return buffer;
}

template<typename PayloadType>
Packet<PayloadType> Packet<PayloadType>::deserialize(const bytearray& data) {
    Packet<PayloadType> packet;
    memcpy(&packet.payload, data.data() + sizeof(PacketHeader), sizeof(PayloadType));
    return packet;
}

template<typename T>
void ThreadSafeQueue<T>::push(T item) {
    std::lock_guard<std::mutex> lock(mutex);
    queue.push(std::move(item));
}

template<typename T>
bool ThreadSafeQueue<T>::pop(T& item) {
    std::lock_guard<std::mutex> lock(mutex);
    if (queue.empty()) return false;
    item = std::move(queue.front());
    queue.pop();
    return true;
}

void SocketServer::initialize_socket() {
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) return;

    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config.port);

    auto _ = bind(server_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address));
    listen(server_socket, config.max_connections);
}

void SocketServer::accept_connections() {
    while (running) {
        sockaddr_in client_addr = {};
        SockLenType client_len = sizeof(client_addr);

        SocketType client_socket = accept(
            server_socket, 
            reinterpret_cast<sockaddr*>(&client_addr), 
            &client_len
        );

        if (client_socket != INVALID_SOCKET) {
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));

            ClientInfo client{
                client_socket, 
                std::string(ip), 
                ntohs(client_addr.sin_port)
            };

            {
                std::lock_guard<std::mutex> lock(clients_mutex);
                clients.push_back(client);
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void SocketServer::process_packets() {
    while (running) {
        std::pair<ClientInfo, bytearray> packet;
        if (packet_queue.pop(packet)) {
            PacketHeader header;
            memcpy(&header, packet.second.data(), sizeof(PacketHeader));

            auto handler = handlers.find(header.type);
            if (handler != handlers.end()) {
                handler->second(packet.second);
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

SocketServer::SocketServer(const ServerConfig& cfg) : config(cfg) {
    initialize_socket();
}

void SocketServer::start() {
    running = true;
    accept_thread = std::make_unique<std::thread>(&SocketServer::accept_connections, this);
    process_thread = std::make_unique<std::thread>(&SocketServer::process_packets, this);
}

void SocketServer::stop() {
    running = false;
    if (accept_thread) accept_thread->join();
    if (process_thread) process_thread->join();

    close(server_socket);
}

SocketServer::~SocketServer() {
    stop();
}

