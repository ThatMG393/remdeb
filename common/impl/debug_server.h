#pragma once

#include <sys/types.h>
#include <vector>

typedef std::vector<u_int8_t> bytearray;

class DebugServer {
public:
    DebugServer(int port = 8088);

    void start();
    void stop();

private:
    bytearray readMemory(u_int32_t address);
    void writeMemory(u_int32_t address, bytearray data);
};
