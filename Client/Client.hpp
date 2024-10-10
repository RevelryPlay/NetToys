
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <queue>
#include <unistd.h>
#include <ArgoDraft/ArgoLogger.hpp>

#ifdef __WIN32__
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
#endif

class Client {
    public:
        Client();
        explicit Client(int port);
        Client(int port, const std::string &host);
        Client(int port, const std::string &host, int bufferSize);

private:
    int port = 9876;
    std::string host;
    int bufferSize = 512;

    unsigned long long clientSocket = -1;

    std::string logFileName = "NetToys.log";
    ArgoDraft::Logger logger = ArgoDraft::Logger(logFileName);

    // A queue of messages to be sent
    std::queue<std::string> messageQueue;

    auto CreateSocket() -> void;
    auto Connect() -> void;
    auto Send() -> void;
    auto Receive() const -> void;
    auto Close() -> void;
    auto Run() -> void;
};

#endif //CLIENT_HPP
