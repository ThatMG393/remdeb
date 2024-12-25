#pragma once

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <linux/types.h>
#include <vector>

typedef u_int8_t byte;
typedef std::vector<byte> bytearray;
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
	static Packet<PayloadType> deserialize(const bytearray& data) {
		PayloadType recievedPayload;
		memcpy(&recievedPayload, data.data(), sizeof(PayloadType));

		return Packet<PayloadType>(recievedPayload);
	}

	const PayloadType payload;

	bytearray serialize() const {
		byte* ptr = reinterpret_cast<byte*>(payload);
		bytearray buf = bytearray(ptr, ptr + sizeof(PayloadType));

		return buf;
	}

private:
	Packet(PayloadType payload) : payload(payload) { }
};
