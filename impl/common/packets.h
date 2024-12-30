#pragma once

#include "net/packet/packet.h"
#include "memory.h"

#define PT_C2S_OFFSET(t) t + 32767

namespace C2S { // <32767
	static PacketType GetBaseAddressPacketType = 1;
	PACKED_STRUCT GetBaseAddressPayload { }; // may act like a hello packet

    static PacketType ReadMemPacketType = 2; 	
	PACKED_STRUCT ReadMemPayload {
		address mem_address;
	};

   	static PacketType WriteMemPacketType = 3;
	PACKED_STRUCT WriteMemPayload {
		address mem_address;
		net_bytearray data;
	};
}

namespace S2C { // >32767
	static PacketType GetBaseAddressPacketType = PT_C2S_OFFSET(1);
	PACKED_STRUCT GetBaseAddressPayload {
		address base_address;
	};

   	static PacketType ReadMemPacketType = 2 + PT_C2S_OFFSET(2);
	PACKED_STRUCT ReadMemPayload {
		net_bytearray data;
	};
}
