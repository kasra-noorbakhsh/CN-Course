#include <iostream>
#include <queue>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <mutex>

#pragma comment(lib, "ws2_32.lib")

#define PACKET_SIZE 1500

using namespace std;

queue<pair<char*, int>> packet_queue;
mutex mtx;

void forward_packets(SOCKET sock, sockaddr_in &receiver_addr) {
    while (true) {
        unique_lock<mutex> lock(mtx);
        if (!packet_queue.empty()) {
            auto packet = packet_queue.front();
            packet_queue.pop();
            lock.unlock();
            sendto(sock, packet.first, packet.second, 0, (sockaddr*)&receiver_addr, sizeof(receiver_addr));
            cout << "Forwarded packet with size " << packet.second << endl;
            delete[] packet.first;
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in router_addr, receiver_addr;
    router_addr.sin_family = AF_INET;
    router_addr.sin_port = htons(12345);
    router_addr.sin_addr.s_addr = INADDR_ANY;

    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(12346);
    inet_pton(AF_INET, "127.0.0.1", &receiver_addr.sin_addr);

    bind(sock, (sockaddr*)&router_addr, sizeof(router_addr));

    thread forward_thread(forward_packets, sock, ref(receiver_addr));

    char buffer[PACKET_SIZE + 4];
    sockaddr_in sender_addr;
    int len = sizeof(sender_addr);

    while (true) {
        int recv_len = recvfrom(sock, buffer, PACKET_SIZE + 4, 0, (sockaddr*)&sender_addr, &len);
        if (recv_len > 0) {
            char* packet = new char[recv_len];
            memcpy(packet, buffer, recv_len);

            lock_guard<mutex> lock(mtx);
            packet_queue.push(make_pair(packet, recv_len));
            cout << "Received packet with size " << recv_len << endl;
        }
    }

    forward_thread.join();
    closesocket(sock);
    WSACleanup();
    return 0;
}
