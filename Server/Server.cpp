#include "Server.hpp"

#include <ArgoDraft/ArgoLogger.hpp>

Server::Server() {
    this->host = "127.0.0.1";

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage("Server starting on default port 9876", ArgoDraft::LogLevel::INFO);

    this->Run();
}

Server::Server(const int port) {
    this->port = port;
    this->host = "127.0.0.1";

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage(("Server starting on port " + std::to_string(port)).c_str(), ArgoDraft::LogLevel::INFO);

    this->Run();
}

Server::Server(const int port, const std::string &host) {
    this->port = port;
    this->host = host;

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage(("Server starting on port " + std::to_string(port) + " with host " + host).c_str(),
                            ArgoDraft::LogLevel::INFO);

    this->Run();
}

Server::Server(const int port, const std::string &host, const int maxConnections, const int maxClients) {
    this->port = port;
    this->host = host;
    this->maxConnections = maxConnections;
    this->maxClients = maxClients;

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage(
        ("Server starting on port " + std::to_string(port) + " with host " + host + " with max connections " +
         std::to_string(maxConnections) + " and max clients " + std::to_string(maxClients)).c_str(),
        ArgoDraft::LogLevel::INFO);

    this->Run();
}

Server::Server(const int port, const std::string &host, const int maxConnections, const int maxClients,
               const int bufferSize) {
    this->port = port;
    this->host = host;
    this->maxConnections = maxConnections;
    this->maxClients = maxClients;
    this->bufferSize = bufferSize;

    this->logger.LogMessage(
        ("Server starting on port " + std::to_string(port) + " with host " + host + " with max connections " +
         std::to_string(maxConnections) + " and max clients " + std::to_string(maxClients) + " with buffer size " +
         std::to_string(bufferSize)).c_str(), ArgoDraft::LogLevel::INFO);

    this->Run();
}

auto Server::CreateSocket() -> void {
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->serverSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        this->logger.LogMessage("Failed to create server socket", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    this->logger.LogMessage("Server socket created", ArgoDraft::LogLevel::INFO);
}

auto Server::BindSocket() const -> void {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->host.c_str());

    if (const int bindResults = bind(this->serverSocket, reinterpret_cast<sockaddr *>(&serverAddress),
                                     sizeof(serverAddress));
        bindResults < 0) {
        std::cerr << "Failed to bind socket" << std::endl;
        this->logger.LogMessage("Failed to bind server socket", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    this->logger.LogMessage("Server socket bound", ArgoDraft::LogLevel::INFO);
}

auto Server::Listen() const -> void {
    if (const int listResults = listen(this->serverSocket, this->maxConnections); listResults < 0) {
        std::cerr << "Failed to listen on socket" << std::endl;
        this->logger.LogMessage("Failed to listen on server socket", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    this->logger.LogMessage("Server socket listening", ArgoDraft::LogLevel::INFO);
}

auto Server::Accept() -> void {
    this->clientSocket = accept(this->serverSocket, nullptr, nullptr);
    if (this->clientSocket < 0) {
        std::cerr << "Failed to accept connection" << std::endl;
        this->logger.LogMessage("Failed to accept connection", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    this->logger.LogMessage("Server socket accepted connection", ArgoDraft::LogLevel::INFO);
}

auto Server::Receive() -> void {
    char buffer[this->bufferSize] = {0};
    recv(this->clientSocket, buffer, sizeof(buffer), 0);

    // If buffer is empty or contains "//exit", close the client socket and return otherwise log the message
    if (std::string(buffer).empty() || std::string(buffer) == "//exit") {
        this->CloseClient();
        return;
    }

    this->logger.LogMessage(("Server Received: " + std::string(buffer)).c_str(), ArgoDraft::LogLevel::INFO);
}

auto Server::Send() const -> void {
    constexpr char message[] = "Server Hello!";
    send(this->clientSocket, message, sizeof(message), 0);
    this->logger.LogMessage(("Server Sent: " + std::string(message)).c_str(), ArgoDraft::LogLevel::INFO);
}

auto Server::Close() -> void {
    if (this->clientSocket > 0) {
        this->CloseClient();
    }

    if (this->serverSocket < 0) {
        return;
    }

    if (const int closeResults = close(this->serverSocket); closeResults < 0) {
        std::cerr << "Failed to close socket" << std::endl;
        this->logger.LogMessage("Failed to close server socket", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    this->serverSocket = 0;
    this->logger.LogMessage("Server socket closed", ArgoDraft::LogLevel::INFO);
}

auto Server::CloseClient() -> void {
    if (this->clientSocket < 0) {
        return;
    }

    // if (const int closeResults = close(this->clientSocket); closeResults < 0) {
    //     std::cerr << "Server failed to close client socket" << std::endl;
    //     this->logger.LogMessage("Server failed to close client socket", ArgoDraft::LogLevel::CRITICAL);
    //     return;
    // }

    this->clientSocket = 0;
    this->logger.LogMessage("Server client socket closed", ArgoDraft::LogLevel::INFO);
}

auto Server::Run() -> void {
    this->logger.LogMessage("Starting server", ArgoDraft::LogLevel::INFO);

#ifdef __WIN32__
    this->logger.LogMessage("Starting Windows Sockets", ArgoDraft::LogLevel::DEBUG);
    constexpr WORD versionWanted = MAKEWORD(1, 1);
    WSADATA wsaData;
    if (const int WASockResult = WSAStartup(versionWanted, &wsaData); WASockResult != 0) {
        std::cerr << "WSAStartup failed with error: " << WASockResult << std::endl;
        this->logger.LogMessage("Server WSAStartup failed", ArgoDraft::LogLevel::CRITICAL);
        return;
    }
    this->logger.LogMessage("Windows Sockets started", ArgoDraft::LogLevel::DEBUG);
#endif
    this->CreateSocket();
    this->BindSocket();

    this->Listen();
    this->Accept();

    while (this->clientSocket > 0) {
        this->Receive();
        // this->Send();
    }

    // if (this->serverSocket > 0) {
    this->Close();
    // }

#ifdef __WIN32__
    this->logger.LogMessage("Cleaning up Windows Sockets", ArgoDraft::LogLevel::DEBUG);
    WSACleanup();
#endif
}
