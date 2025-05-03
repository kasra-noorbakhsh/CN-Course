#include "server.h"

TCPServer::TCPServer(int port) : port(port), addrlen(sizeof(address)) {

    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket == INVALID_SOCKET) {
        perror("socket failed");
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) == SOCKET_ERROR) {
        perror("bind failed");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) == SOCKET_ERROR) {
        perror("listen");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

}

TCPServer::~TCPServer() {

    closesocket(server_socket);
    WSACleanup();

    for (auto &thread : clientThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

}

void TCPServer::start() {

    cout << "Server started on port " << port << endl;
    SOCKET client_socket;

    while ((client_socket = accept(server_socket, (struct sockaddr *)&address, &addrlen)) != INVALID_SOCKET) {
        cout << "Connection accepted" << endl;
        lock_guard<mutex> lock(mtx);
        clientThreads.emplace_back(&TCPServer::handleClient, this, client_socket);
    }

    if (client_socket == INVALID_SOCKET) {
        perror("accept");
        closesocket(server_socket);
        WSACleanup();
        exit(EXIT_FAILURE);
    }

}

void TCPServer::handleClient(SOCKET client_socket) {

    if (!performHandshake(client_socket)) {
        closesocket(client_socket);
        return;
    }

    receiveData(client_socket);
    closesocket(client_socket);

}

bool TCPServer::performHandshake(SOCKET client_socket) {

    char buffer[1024] = {0};

    // Receive SYN
    int valread = recv(client_socket, buffer, 1024, 0);

    if (valread > 0 &&string(buffer) == "SYN") {
        cout << "SYN received" <<endl;
        send(client_socket, "SYN-ACK", 7, 0);
        cout << "SYN-ACK sent" <<endl;

        // Receive ACK
        valread = recv(client_socket, buffer, 1024, 0);

        if (valread > 0 &&string(buffer) == "ACK") {
           cout << "ACK received, handshake completed" <<endl;
            return true;
        }

    }

    return false;

}

void TCPServer::receiveData(SOCKET client_socket) {

    char buffer[1024] = {0};
    int valread;

    while ((valread = recv(client_socket, buffer, 1024, 0)) > 0) {
        cout << "Data received: " << buffer << endl;
        send(client_socket, "ACK", 3, 0);
        cout << "ACK sent" << endl;
        memset(buffer, 0, 1024); // Clear the buffer
    }

}