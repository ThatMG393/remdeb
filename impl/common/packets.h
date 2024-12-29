#pragma once

#include "net/packet/packet.h"
#include "memory.h"

namespace C2S { // <32767
    static PacketType ReadMemPacketType = 1; 	
	PACKED_STRUCT ReadMemPayload {
		address mem_address;
	};

   	static PacketType WriteMemPacketType = 2;
	PACKED_STRUCT WriteMemPayload {
		address mem_address;
		net_bytearray data;
	};
}

namespace S2C { // >32767
   	static PacketType ReadMemPacketType = 32767;
	PACKED_STRUCT ReadMemPayload {
		net_bytearray data;
	};
}
