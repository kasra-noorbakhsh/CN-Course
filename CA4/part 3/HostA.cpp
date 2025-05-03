#include <iostream>
#include <fstream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
#include <vector>
#include <mutex>
#include <condition_variable>

#pragma comment(lib, "ws2_32.lib")

#define PACKET_SIZE 1500
#define WINDOW_SIZE 5
#define TIMEOUT_MS 1000

using namespace std;

mutex mtx;
condition_variable cv;
bool ack_received[WINDOW_SIZE] = {false};
bool done = false;

void receive_acks(SOCKET sock, sockaddr_in &router_addr, int &base) {
    char ack_buffer[4];
    while (!done) {
        int len = sizeof(router_addr);
        int recv_len = recvfrom(sock, ack_buffer, 4, 0, (sockaddr*)&router_addr, &len);
        if (recv_len > 0) {
            int ack_num = *((int*)ack_buffer);
            lock_guard<mutex> lock(mtx);
            cout << "Received ACK for packet " << ack_num << endl;
            if (ack_num >= base) {
                ack_received[ack_num % WINDOW_SIZE] = true;
                cv.notify_all();
            }
        }
    }
}

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in router_addr;
    router_addr.sin_family = AF_INET;
    router_addr.sin_port = htons(12345);
    inet_pton(AF_INET, "127.0.0.1", &router_addr.sin_addr);

    ifstream file("input.txt", ios::binary);
    if (!file.is_open()) {
        cerr << "Could not open file!" << endl;
        return 1;
    }

    vector<thread> threads;
    int base = 0;
    threads.emplace_back(receive_acks, sock, ref(router_addr), ref(base));

    char buffer[PACKET_SIZE];
    int seq_num = 0;

    while (!file.eof() || seq_num != base) {
        while (seq_num < base + WINDOW_SIZE && file.read(buffer, PACKET_SIZE)) {
            int packet_size = file.gcount();
            char packet[PACKET_SIZE + 4];
            memcpy(packet, &seq_num, 4);
            memcpy(packet + 4, buffer, packet_size);

            sendto(sock, packet, packet_size + 4, 0, (sockaddr*)&router_addr, sizeof(router_addr));
            cout << "Sent packet " << seq_num << endl;
            seq_num++;
        }

        unique_lock<mutex> lock(mtx);
        if (cv.wait_for(lock, chrono::milliseconds(TIMEOUT_MS), [&]() { return ack_received[base % WINDOW_SIZE]; })) {
            ack_received[base % WINDOW_SIZE] = false;
            base++;
        } else {
            seq_num = base;
        }

        if (file.eof() && base == seq_num) {
            done = true;
        }
    }

    for (auto &t : threads) {
        t.join();
    }

    file.close();
    closesocket(sock);
    WSACleanup();
    return 0;
}
