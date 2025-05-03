#ifndef CLIENT_H
#define CLIENT_H

#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

class TCPClient {
public:

    TCPClient(const string &address, int port);
    ~TCPClient();
    void connectToServer();
    void sendMessage(const string &message);

private:

    SOCKET sock;
    struct sockaddr_in serv_addr;

    bool performHandshake();
    bool receiveAck();

    int cwnd; 
    int ssthresh; 
    int dupAcks; 
    int ackedSegments; 
    vector<string> packets; 

    void onAckReceived();
    void onDupAckReceived();
    void onTimeout();
    void sendPacket(const std::string &packet);

};

#endif // CLIENT_H