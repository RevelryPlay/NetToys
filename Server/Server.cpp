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

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

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
    // Listen for incoming connections
    if (const int listenResults = listen(this->serverSocket, this->maxConnections); listenResults < 0) {
        std::cerr << "Failed to listen for connections" << std::endl;
        this->logger.LogMessage("Failed to listen for connections", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    // Log the listening
    this->logger.LogMessage("Server listening for connections", ArgoDraft::LogLevel::INFO);
}

auto Server::Accept() -> void {
    // Create a sockaddr_in struct to hold the client address
    sockaddr_in clientAddress{};
    socklen_t clientAddressLength = sizeof(clientAddress);

    // Accept the incoming connection
    this->clientSocket = accept(this->serverSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressLength);
    if (this->clientSocket < 0) {
        std::cerr << "Failed to accept connection" << std::endl;
        this->logger.LogMessage("Failed to accept connection", ArgoDraft::LogLevel::CRITICAL);
        exit(EXIT_FAILURE);
    }

    // Log the connection
    this->logger.LogMessage("Server accepted connection", ArgoDraft::LogLevel::INFO);
}

auto Server::Receive() const -> void {
    // If the client socket is invalid, return
    if (this->clientSocket < 0) {
        return;
    }

    // Loop to receive messages
    while (this->clientSocket > 0) {
        // Create a buffer to hold the incoming message
        char buffer[this->bufferSize] = {};

        // Receive the message
        if (const int receiveResults = recv(this->clientSocket, buffer, this->bufferSize, 0); receiveResults < 0) {
            std::cerr << "Failed to receive message" << std::endl;
            this->logger.LogMessage("Failed to receive message", ArgoDraft::LogLevel::CRITICAL);
            exit(EXIT_FAILURE);
        }

        // Log the message
        this->logger.LogMessage(("Server received message: " + std::string(buffer)).c_str(), ArgoDraft::LogLevel::INFO);
    }
}

auto Server::Send() -> void {
    // If the message queue is empty, return
    if (this->messageQueue.empty() || this->clientSocket < 0) {
        return;
    }

    while (!this->messageQueue.empty()) {
        // Get the message from the front of the queue
        const std::string message = this->messageQueue.front();

        // Filter out internal commands
        if (message != "//exit" && message != "//shutdown") {
            // Send the message
            if (const int sendResults = send(this->clientSocket, message.c_str(), message.length(), 0); sendResults < 0) {
                std::cerr << "Failed to send message" << std::endl;
                this->logger.LogMessage("Failed to send message", ArgoDraft::LogLevel::CRITICAL);
                exit(EXIT_FAILURE);
            }

            // Log the message
            this->logger.LogMessage(("Server sent message: " + message).c_str(), ArgoDraft::LogLevel::INFO);

            // Remove the message from the queue
            this->messageQueue.pop();
        }

        // If the message was "//exit", close the client socket
        if (message == "//exit") {
            this->CloseClient();
        }

        // If the message was "//shutdown", close the server socket
        if (message == "//shutdown") {
            this->Close();
        }
    }
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

    if (const int closeResults = close(this->clientSocket); closeResults < 0) {
        std::cerr << "Server failed to close client socket" << std::endl;
        this->logger.LogMessage("Server failed to close client socket", ArgoDraft::LogLevel::CRITICAL);
        return;
    }

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

    this->messageQueue.emplace("Server Hello!");
    this->messageQueue.emplace("A second message from the server.");
    this->messageQueue.emplace("A third message from the server.");

    // Create the server socket
    this->CreateSocket();
    this->BindSocket();

    // Listen for incoming connections
    this->Listen();

    // Accept incoming connections
    this->Accept();

    // If the client socket is valid, receive and send messages
    if (this->clientSocket > 0) {
        // Ideally send and receive would be in separate threads or non-blocking calls
        // For now, we'll just call them in sequence
        // The client and server should have these in opposite order for now
        this->Receive();
        this->Send();
    }

    // If all clients have disconnected, close the server socket
    // this->Close();

#ifdef __WIN32__
    this->logger.LogMessage("Cleaning up Windows Sockets", ArgoDraft::LogLevel::DEBUG);
    WSACleanup();
#endif
}
