#include "impl/server/debug_server.h"

int main() {
    DebugServer dbs {};
    dbs.start();

    while(true){}
}

