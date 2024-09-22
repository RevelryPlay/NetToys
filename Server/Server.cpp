#include "Server.hpp"

#include <ArgoDraft/ArgoLogger.hpp>

Server::Server() {
    ArgoDraft::Logger::LogMessage("Starting server on default port 9876", "NetToys.log");

    this->host = "127.0.0.1";
    this->Run();
}

Server::Server(const int port) {
    ArgoDraft::Logger::LogMessage(("Starting server on port " + std::to_string(port)).c_str(), "NetToys.log");

    this->port = port;
    this->host = "127.0.0.1";
    this->Run();
}

Server::Server(const int port, const std::string &host) {
    ArgoDraft::Logger::LogMessage(("Starting server on port " + std::to_string(port) + " with host " + host).c_str(),
                                  "NetToys.log");

    this->port = port;
    this->host = host;
    this->Run();
}

Server::Server(const int port, const std::string &host, const int maxConnections, const int maxClients) {
    ArgoDraft::Logger::LogMessage(
        ("Starting server on port " + std::to_string(port) + " with host " + host + " with max connections " +
         std::to_string(maxConnections) + " and max clients " + std::to_string(maxClients)).c_str(),
        "NetToys.log");

    this->port = port;
    this->host = host;
    this->maxConnections = maxConnections;
    this->maxClients = maxClients;
    this->Run();
}

Server::Server(const int port, const std::string &host, const int maxConnections, const int maxClients,
               const int bufferSize) {
    ArgoDraft::Logger::LogMessage(
        ("Starting server on port " + std::to_string(port) + " with host " + host + " with max connections " +
         std::to_string(maxConnections) + " and max clients " + std::to_string(maxClients) + " with buffer size " +
         std::to_string(bufferSize)).c_str(), "NetToys.log");

    this->port = port;
    this->host = host;
    this->maxConnections = maxConnections;
    this->maxClients = maxClients;
    this->bufferSize = bufferSize;
    this->Run();
}

auto Server::CreateSocket() -> void {
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to create socket", "NetToys.log");
        exit(EXIT_FAILURE);
    }

    ArgoDraft::Logger::LogMessage("Socket created", "NetToys.log");
}

auto Server::BindSocket() const -> void {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->host.c_str());

    if (const int bindResults = bind(this->serverSocket, (sockaddr *) &serverAddress, sizeof(serverAddress));
        bindResults < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to bind socket", "NetToys.log");
        exit(EXIT_FAILURE);
    }

    ArgoDraft::Logger::LogMessage("Socket bound", "NetToys.log");
}

auto Server::Listen() const -> void {
    if (const int listResults = listen(this->serverSocket, this->maxConnections); listResults < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to listen on socket", "NetToys.log");
        exit(EXIT_FAILURE);
    }

    ArgoDraft::Logger::LogMessage("Listening on socket", "NetToys.log");
}

auto Server::Accept() -> void {
    this->clientSocket = accept(this->serverSocket, nullptr, nullptr);
    if (this->clientSocket < 0) {
        std::cerr << "Failed to accept connection" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to accept connection", "NetToys.log");
        exit(EXIT_FAILURE);
    }

    ArgoDraft::Logger::LogMessage("Connection accepted", "NetToys.log");
}

auto Server::Receive() const -> void {
    char buffer[this->bufferSize] = {0};
    recv(this->clientSocket, buffer, sizeof(buffer), 0);
    std::cout << "Received: " << buffer << std::endl;
    ArgoDraft::Logger::LogMessage(("Received: " + std::string(buffer)).c_str(), "NetToys.log");
}

auto Server::Send() const -> void {
    constexpr char message[] = "Server Hello!";
    send(this->clientSocket, message, sizeof(message), 0);
    std::cout << "Sent: " << message << std::endl;
    ArgoDraft::Logger::LogMessage(("Sent: " + std::string(message)).c_str(), "NetToys.log");
}

auto Server::Close() const -> void {
    if (const int closeResults = close(this->serverSocket); closeResults < 0) {
        std::cerr << "Failed to close socket" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to close socket", "NetToys.log");
        exit(EXIT_FAILURE);
    }

    ArgoDraft::Logger::LogMessage("Socket closed", "NetToys.log");
}

auto Server::Run() -> void {
    ArgoDraft::Logger::LogMessage("Attempting to run server", "NetToys.log");

#ifdef __WIN32__
    constexpr WORD versionWanted = MAKEWORD(1, 1);
    WSADATA wsaData;
    if (const int WASockResult = WSAStartup(versionWanted, &wsaData); WASockResult != 0) {
        std::cerr << "WSAStartup failed with error: " << WASockResult << std::endl;
        return;
    }
#endif
    this->CreateSocket();
    this->BindSocket();

    this->Listen();
    this->Accept();

    if (this->clientSocket > 0) {
        this->Receive();
        this->Send();
    }

    if (this->serverSocket > 0) {
        this->Close();
    }

#ifdef __WIN32__
    WSACleanup();
#endif
}
