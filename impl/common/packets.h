#pragma once

#include "socket/socket.h"
#include "packet/packet.h"
#include <vector>

typedef std::vector<u_int8_t> bytearray;

struct ReadMemPayload {
	u_int32_t address;
};

struct WriteMemPayload {
	u_int32_t address;
	bytearray data;
};

class ReadMemPacket : Packet<ReadMemPayload> {
public:
	static constexpr PacketType Type = 1;
};

class WriteMemPacket : Packet<WriteMemPayload> {
public:
	static constexpr PacketType Type = 2;
};
