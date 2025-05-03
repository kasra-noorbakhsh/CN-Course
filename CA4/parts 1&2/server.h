#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <mutex>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class TCPServer {

public:

    TCPServer(int port);
    ~TCPServer();
    void start();

private:

    int port;
    mutex mtx;
    int addrlen;
    SOCKET server_socket;
    struct sockaddr_in address;
    vector<thread> clientThreads;

    void handleClient(SOCKET client_socket);
    bool performHandshake(SOCKET client_socket);
    void receiveData(SOCKET client_socket);

};

#endif // SERVER_H