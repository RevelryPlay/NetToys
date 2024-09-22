#ifndef SERVER_HPP
#define SERVER_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#ifdef __WIN32__
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
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
    int bufferSize = 1024;

    int serverSocket = 0;
    int clientSocket = 0;

    auto CreateSocket() -> void;
    auto BindSocket() const -> void;
    auto Listen() const -> void;
    auto Accept() -> void;
    auto Receive() const -> void;
    auto Send() const -> void;
    auto Close() const -> void;
    auto Run() -> void;
};

#endif //SERVER_HPP
