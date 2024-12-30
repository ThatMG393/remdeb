#pragma once

#include "net/packet/packet.h"
#include "impl/common/logger.h"
#include <csetjmp>
#include <csignal>
#include <iomanip>
#include <optional>
#include <sstream>
#include <inttypes.h>
#include <string>

typedef uintptr_t address;

class MemoryReader {
private:
    static sigjmp_buf jump_buffer;
    static volatile sig_atomic_t canAccessMemory;
    static address lastFailedAddress;

    static void segfault_handler(int sig) {
		Logger::getLogger()->warn("Supressed SIGSEGV because of the access attempt to " + intToHex(lastFailedAddress));

        canAccessMemory = 0;
        siglongjmp(jump_buffer, 1);
    }

public:
    static std::optional<byte> readByte(address addr) {
    	auto b = readBytes(addr, 1);
    	return (b.has_value()) ? std::optional<byte>(b->at(0)) : std::nullopt;
    }

    static std::optional<bytearray> readBytes(address addr, size_t count) {
        lastFailedAddress = addr;
        signal(SIGSEGV, segfault_handler);
        
        if (sigsetjmp(jump_buffer, 1) == 0) {
            canAccessMemory = 1;
            bytearray bytes(count);
            std::memcpy(bytes.data(), reinterpret_cast<void*>(addr), count);
			Logger::getLogger()->info(std::to_string(bytes.size()));
            return bytes;
        }

        signal(SIGSEGV, SIG_DFL);
        return std::nullopt;
    }

    static bool isValidMemoryAddress(address addr, size_t size = 1) {
        try {
            auto result = readBytes(addr, size);
            return result.has_value();
        } catch (...) {
            return false;
        }
    }

	// https://stackoverflow.com/a/5100745
	template<typename T>
	static std::string intToHex(T i) {
  		std::stringstream stream;
  		stream << "0x" 
    	 	 << std::setfill('0') << std::setw(sizeof(T)*2) 
         	 << std::hex << i;
  		return stream.str();
	}

    static address getLastFailedAddress() {
        return lastFailedAddress;
    }

	static address getProcessBaseAddress() {
#ifdef __ANDROID__
    	FILE* fp = fopen("/proc/self/maps", "r");
    	if (!fp) return 0;
    	
    	address baseAddr;
    	fscanf(fp, "%" SCNxPTR, &baseAddr);
    	fclose(fp);
    	return baseAddr;
#else
	    dl_phdr_info info;
	    dlinfo(RTLD_SELF, RTLD_DI_LINKMAP, &info);
	    return info.dlpi_addr;
#endif
	}
};

inline sigjmp_buf MemoryReader::jump_buffer;
inline volatile sig_atomic_t MemoryReader::canAccessMemory = 0;
inline address MemoryReader::lastFailedAddress = 0;
