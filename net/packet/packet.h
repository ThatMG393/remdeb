#pragma once

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <linux/types.h>
#include <vector>

typedef std::vector<u_int8_t> bytearray;
typedef u_int16_t PacketType;
typedef size_t PacketSize;

struct __attribute__((packed)) PacketHeader {
	PacketType type;
	PacketSize size;
};

template<typename PayloadType>
class Packet {
public:
	static constexpr PacketType Type = 0;
	static PayloadType deserialize(const bytearray& data) {
		PayloadType recievedPayload;
		memcpy(&recievedPayload, data.data() + sizeof(PacketHeader), sizeof(PayloadType));

		return recievedPayload;
	}

	void serialize();
};
