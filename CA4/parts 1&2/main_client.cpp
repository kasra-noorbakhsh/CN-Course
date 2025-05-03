#include "client.h"

int main() {

    TCPClient client("127.0.0.1", 8080);
    client.connectToServer();
    client.sendMessage("Hello from client");
    client.sendMessage("This is a PACKET");
    client.sendMessage("Another packet");
    return 0;

}