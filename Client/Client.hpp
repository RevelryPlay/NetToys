
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <cstring>
#include <iostream>
#include <unistd.h>

#ifdef __WIN32__
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #include <windows.h>
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
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
    int bufferSize = 1024;

    int clientSocket = 0;

    auto CreateSocket() -> void;
    auto Connect() const -> void;
    auto Send(const char* message) const -> void;
    auto Receive() const -> void;
    auto Close() const -> void;
    auto Run() -> void;
};

#endif //CLIENT_HPP
