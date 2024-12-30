#include "impl/client/debug_client.h"
#include "impl/server/debug_server.h"
#include <iostream>
#include <string>

namespace std {
	static inline std::string trim(std::string s) {
    	s.erase(
    		s.begin(),
    		std::find_if(
    			s.begin(),
    			s.end(),
    			[](int c) {
    				return !std::isspace(c);
    			}
    		)
    	);
    	return s;
	}
};

int main() {
main:
	std::string side;
	std::cout << "Which side would you want to launch (server [default]/client): ";
	while (true) {
		std::getline(std::cin, side);
		std::trim(side);
		if (side == "server" || side == "client" || side == "") break;
		std::cout << "Invalid choice! Retry!" << std::endl;
	}

	if (side == "" || side == "server") {
		DebugServer ds { };
		ds.start();

		while (true) { }
	} else if (side == "client") {
		DebugClient dc { };
		dc.connectToServer();

		while (true) { }
	} else {
		goto main;
	}
}
