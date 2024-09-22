#include "Client.hpp"
#include <ArgoDraft/ArgoLogger.hpp>

Client::Client() {
    ArgoDraft::Logger::LogMessage("Client starting on default port 9876", "NetToys.log");

    this->host = "127.0.0.1";

    this->Run();
}

Client::Client(const int port) {
    ArgoDraft::Logger::LogMessage(("Client starting on port " + std::to_string(port)).c_str(), "NetToys.log");
    this->port = port;
    this->host = "127.0.0.1";

    this->Run();
}

Client::Client(const int port, const std::string &host) {
    ArgoDraft::Logger::LogMessage(("Client starting on port " + std::to_string(port) + " with host " + host).c_str(),
                                  "NetToys.log");
    this->port = port;
    this->host = host;

    this->Run();
}

Client::Client(const int port, const std::string &host, const int bufferSize) {
    ArgoDraft::Logger::LogMessage(
        ("Client starting on port " + std::to_string(port) + " with host " + host + " with buffer size " +
         std::to_string(bufferSize)).c_str(), "NetToys.log");
    this->port = port;
    this->host = host;
    this->bufferSize = bufferSize;

    this->Run();
}

auto Client::CreateSocket() -> void {
    this->clientSocket = clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to create client socket", "NetToys.log");
        return;
    }

    ArgoDraft::Logger::LogMessage("Client socket created", "NetToys.log");
}

auto Client::Connect() const -> void {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->host.c_str());

    if (connect(this->clientSocket, (sockaddr *) &serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to connect to server", "NetToys.log");
        this->Close();
        return;
    }

    ArgoDraft::Logger::LogMessage("Connected to server", "NetToys.log");
}

auto Client::Send(const char *message) const -> void {
    if (send(this->clientSocket, message, sizeof(message), 0) < 0) {
        std::cerr << "Failed to send message" << std::endl;
        ArgoDraft::Logger::LogMessage("Client failed to send message", "NetToys.log");
        return;
    }

    ArgoDraft::Logger::LogMessage(("Client sent message: " + std::string(message)).c_str(), "NetToys.log");
}

auto Client::Receive() const -> void {
    char buffer[this->bufferSize] = {0};
    if (recv(this->clientSocket, buffer, sizeof(buffer), 0) < 0) {
        std::cerr << "Client failed to receive message" << std::endl;
        ArgoDraft::Logger::LogMessage("Failed to receive message", "NetToys.log");
        return;
    }

    ArgoDraft::Logger::LogMessage(("Client received message: " + std::string(buffer)).c_str(), "NetToys.log");
}

auto Client::Close() const -> void {
    close(this->clientSocket);
    ArgoDraft::Logger::LogMessage("Client closed", "NetToys.log");
}

auto Client::Run() -> void {
#ifdef __WIN32__
    constexpr WORD versionWanted = MAKEWORD(1, 1);
    WSADATA wsaData;
    if (const int WASockResult = WSAStartup(versionWanted, &wsaData); WASockResult != 0) {
        std::cerr << "WSAStartup failed with error: " << WASockResult << std::endl;
        return;
    }
#endif

    this->CreateSocket();

    this->Connect();

    if (this->clientSocket > 0) {
        this->Send("Client Hello!");
        this->Receive();

        this->Close();
    }

#ifdef __WIN32__
    WSACleanup();
#endif
}
