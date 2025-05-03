#include "server.h"

int main() {

    TCPServer server(8080);
    server.start();
    return 0;
    
}