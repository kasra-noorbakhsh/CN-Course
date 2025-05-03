#include "client.h"

TCPClient::TCPClient(const string &address, int port)
    : cwnd(1), ssthresh(64), dupAcks(0), ackedSegments(0) {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Socket creation error");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        closesocket(sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

}

TCPClient::~TCPClient() {

    closesocket(sock);
    WSACleanup();

}

void TCPClient::connectToServer() {

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        closesocket(sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    cout << "Connected to server" << endl;

    if (!performHandshake()) {
        closesocket(sock);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

}

void TCPClient::sendMessage(const string &message) {

    packets.push_back(message); 
    sendPacket(message); 

    for (int i = 0; i < 3; ++i) { // Try receiving ACK up to 3 times
        if (receiveAck()) {
            cout << "ACK received from server" << endl;
            onAckReceived();
            break;
        } else {
            cout << "ACK not received from server" << endl;
            onTimeout();
        }
    }

}

bool TCPClient::performHandshake() {

    // Send SYN
    send(sock, "SYN", 3, 0);
    cout << "SYN sent" << endl;

    // Receive SYN-ACK
    char buffer[1024] = {0};
    int valread = recv(sock, buffer, 1024, 0);

    if (valread > 0 && string(buffer) == "SYN-ACK") {
        cout << "SYN-ACK received" << endl;

        // Send ACK
        send(sock, "ACK", 3, 0);
        cout << "ACK sent, handshake completed" << endl;
        return true;
    }

    return false;

}

bool TCPClient::receiveAck() {

    char buffer[1024] = {0};
    int valread = recv(sock, buffer, 1024, 0);

    if (valread > 0 && string(buffer) == "ACK") {
        return true;
    } else if (valread > 0) {
        string ack(buffer);
        if (ack == "DUP-ACK") {
            onDupAckReceived();
        }
    }

    return false;

}

void TCPClient::onAckReceived() {

    ackedSegments++;

    if (cwnd < ssthresh) {
        // Slow Start
        cwnd++;
    } else {
        // Congestion Avoidance
        cwnd += 1 / cwnd;
    }

    cout << "Congestion Window: " << cwnd << endl;

    if (ackedSegments < packets.size()) {
        sendPacket(packets[ackedSegments]); 
    }

}

void TCPClient::onDupAckReceived() {

    dupAcks++;

    if (dupAcks == 3) {
        // Fast Retransmit
        ssthresh = cwnd / 2;
        cwnd = ssthresh + 3;
        cout << "Fast Retransmit: Congestion Window: " << cwnd << endl;
        sendPacket(packets[ackedSegments]); 
    }

}

void TCPClient::onTimeout() {

    ssthresh = cwnd / 2;
    cwnd = 1;
    dupAcks = 0;
    cout << "Timeout: Congestion Window: " << cwnd << endl;
    sendPacket(packets[ackedSegments]);

}

void TCPClient::sendPacket(const string &packet) {

    send(sock, packet.c_str(), packet.size(), 0);
    cout << "Packet sent: " << packet << endl;

}