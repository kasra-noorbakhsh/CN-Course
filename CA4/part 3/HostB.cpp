#include <iostream>
#include <fstream>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

#define PACKET_SIZE 1500

using namespace std;

int main() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    sockaddr_in receiver_addr, sender_addr;
    receiver_addr.sin_family = AF_INET;
    receiver_addr.sin_port = htons(12346);
    receiver_addr.sin_addr.s_addr = INADDR_ANY;

    bind(sock, (sockaddr*)&receiver_addr, sizeof(receiver_addr));

    ofstream file("output.txt", ios::binary);
    if (!file.is_open()) {
        cerr << "Could not open file!" << endl;
        return 1;
    }

    char buffer[PACKET_SIZE + 4];
    int expected_seq_num = 0;
    int len = sizeof(sender_addr);

    while (true) {
        int recv_len = recvfrom(sock, buffer, PACKET_SIZE + 4, 0, (sockaddr*)&sender_addr, &len);
        if (recv_len > 0) {
            int seq_num = *((int*)buffer);
            if (seq_num == expected_seq_num) {
                file.write(buffer + 4, recv_len - 4);
                cout << "Received and wrote packet " << seq_num << " of size " << recv_len - 4 << endl;

                char ack[4];
                memcpy(ack, &seq_num, 4);
                sendto(sock, ack, 4, 0, (sockaddr*)&sender_addr, sizeof(sender_addr));

                expected_seq_num++;
            } else {
                char ack[4];
                int last_ack_num = expected_seq_num - 1;
                memcpy(ack, &last_ack_num, 4);
                sendto(sock, ack, 4, 0, (sockaddr*)&sender_addr, sizeof(sender_addr));
            }
        }

        if (expected_seq_num > 10000) { // Assume we have a mechanism to know the file end
            break;
        }
    }

    file.close();
    closesocket(sock);
    WSACleanup();
    return 0;
}
