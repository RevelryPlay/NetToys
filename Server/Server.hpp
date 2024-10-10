#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <iostream>
#include <queue>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
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

class Server {
public:
    Server();
    explicit Server(int port);
    Server(int port, const std::string &host);
    Server(int port, const std::string &host, int maxConnections, int maxClients);
    Server(int port, const std::string &host, int maxConnections, int maxClients, int bufferSize);

private:
    // Default values to be used if no arguments are passed
    // In the future, these could be read from a configuration file
    int port = 9876;
    std::string host;
    int maxConnections = 5;
    int maxClients = 5;
    int bufferSize = 512;

    unsigned long long serverSocket = 0;
    unsigned long long clientSocket = 0;

    const std::string logFileName = "NetToys.log";
    ArgoDraft::Logger logger = ArgoDraft::Logger(logFileName);

    // A queue of messages to be sent
    std::queue<std::string> messageQueue;

    auto CreateSocket() -> void;
    auto BindSocket() const -> void;
    auto Listen() const -> void;
    auto Accept() -> void;
    auto Receive() const -> void;
    auto Send() -> void;
    auto Close() -> void;
    auto CloseClient() -> void;
    auto Run() -> void;
};

#endif //SERVER_HPP
