# 💻 Computer Networks Projects (CA2 & CA4)

This repository contains coursework from the Computer Networks course, including two comprehensive assignments: **CA2** and **CA4**.

- **CA2** focuses on socket simulation, reliable data transmission, error detection, and evaluating corrupted bit rates.
- **CA4** focuses on implementing real-world networking protocols like TCP and UDP, including congestion and flow control mechanisms.

---

## 📌 Project Overview

- 📘 **CA2 - Socket Simulation & Error Control**

This assignment is divided into 4 parts:

### 🔹 Part 1: Socket Simulation

- Basic implementation of socket-like communication in C++.
- Simulated data transfer between a server and client using TCP.
- Handles connection establishment and message delivery.

### 🔹 Part 2: Reliable Transmission

- Sends data over an unreliable channel with acknowledgments.
- Uses sequence numbers and retransmissions for reliability.

### 🔹 Part 3: Bit Error Evaluation

- Simulates transmission over noisy media.
- Calculates and logs the bit error rate during transfer.
- Helps understand how frequently bits get flipped or lost.

### 🔹 Part 4: Error Detection and Secure Transfer

- Implements checksum/error-detecting logic.
- Ensures that the data integrity is preserved during transmission.

📘 **CA4 - TCP/UDP Protocol Implementation**

### 🔹 Part 1: TCP Client/Server with Three-Way Handshake
- Implements TCP handshake:  
  **SYN → SYN-ACK → ACK**
- Handles multiple clients using multithreading.
- Transfers data packets and verifies delivery via ACKs.

### 🔹 Part 2: TCP Congestion Control - New Reno
- Implements TCP congestion control using the New Reno algorithm.
- Features:
  - Slow Start
  - Congestion Avoidance
  - Fast Retransmit
  - Fast Recovery
- Dynamically adjusts window size based on network conditions.

### 🔹 Part 3: Flow Control - Go-Back-N (UDP)
- Implements flow control using the Go-Back-N protocol over UDP.
- Topology includes:
  - Host A (Sender)
  - Host B (Receiver)
  - Router (Middle node simulating a network)
- Transfers a file (e.g., 1MB) reliably using sequence numbers and window size.
- Handles retransmissions on packet loss.

---

## 🧰 Tools Used

- [Python](https://www.python.org/) for CA2 implementation
- [C/C++](https://en.cppreference.com/w/) for CA4 implementation

---

## 📬 Contact

Made by **Kasra Noorbakhsh**  
📧 Feel free to connect or provide feedback!
