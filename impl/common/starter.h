#pragma once

#include "impl/server/debug_server.h"

namespace RemDebMain {
    static DebugServer* startServer(int port = 8088) {
    	DebugServer* server = new DebugServer(port);
    	server->start();
    	return server;
    }

    static void startClient(int port) { }
}
