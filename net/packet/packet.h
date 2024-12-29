#pragma once

#include "impl/common/logger.h"
#include <cstddef>
#include <cstdint>
#include <optional>
#include <string>
#include <vector>

#define PACKED_STRUCT struct __attribute__((packed))

typedef uint16_t PacketType;
typedef uint32_t PacketSize;

PACKED_STRUCT PacketHeader {
	PacketType type;
	PacketSize size;
};

template<typename T>
PACKED_STRUCT NetworkVector {
	size_t data_size = 0;
    T* data_array = nullptr;

    explicit NetworkVector(std::vector<T> vec) : data_size(vec.size()), data_array(vec.data()) { }

	NetworkVector() : NetworkVector(std::vector<T>()) { }
    NetworkVector(size_t size) : NetworkVector(std::vector<T>(size)) { }
    NetworkVector(T* begin, T* end) : NetworkVector(std::vector<T>(begin, end)) { }
    NetworkVector(std::vector<T> orig, std::vector<T> extra) : NetworkVector(orig.insert(orig.end(), extra.begin(), extra.end())) { }
};

typedef uint8_t byte;
typedef std::vector<byte> bytearray;
typedef NetworkVector<byte> net_bytearray;

class Packet {
public:
	template<typename T>
	static Packet wrap(PacketType type, T structData) {
		byte* dataPtr = reinterpret_cast<byte*>(&structData);
		return Packet(type, bytearray(dataPtr, dataPtr + sizeof(T)));
	}

	const PacketHeader header;
	const bytearray data;

	Packet(PacketHeader header, bytearray data) : header(header), data(data) { }
	Packet(PacketType type, bytearray data) : header({ .type = type, .size = data.size() }), data(data) { }

	bytearray serialize() const {
		bytearray serializedHeader(sizeof(PacketHeader));
		memcpy(serializedHeader.data(), &header, serializedHeader.size());

		serializedHeader.insert(serializedHeader.end(), data.begin(), data.end());
		return serializedHeader;
	}

	template<typename T>
	const std::optional<T> deserialize() const {
		static_assert(std::is_standard_layout_v<T>, "Type must be standard layout");
		static_assert(std::is_trivially_copyable_v<T>, "Type must be trivially copyable");

		if (sizeof(T) != data.size()) {
			Logger::getLogger().error("Error deserializing '" + std::string(typeid(T).name()) + "' because the data size doesn't match! (" + std::to_string(data.size()) + " != " + std::to_string(sizeof(T)) + ")");
			return std::nullopt;
		}
	
		T dataStruct;
		memcpy(&dataStruct, data.data(), sizeof(T));

		return dataStruct;
	}
};
