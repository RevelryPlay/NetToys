#include "Client.hpp"
#include <ArgoDraft/ArgoLogger.hpp>

Client::Client() {
    this->host = "127.0.0.1";

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage("Client starting on default port 9876", ArgoDraft::LogLevel::INFO);

    this->Run();
}

Client::Client(const int port) {
    this->port = port;
    this->host = "127.0.0.1";

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage(("Client starting on port " + std::to_string(port)).c_str(), ArgoDraft::LogLevel::INFO);

    this->Run();
}

Client::Client(const int port, const std::string &host) {
    this->port = port;
    this->host = host;

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage(("Client starting on port " + std::to_string(port) + " with host " + host).c_str(),
                            ArgoDraft::LogLevel::INFO);

    this->Run();
}

Client::Client(const int port, const std::string &host, const int bufferSize) {
    this->port = port;
    this->host = host;
    this->bufferSize = bufferSize;

    this->logger.SetFileLogLevel(ArgoDraft::LogLevel::INFO);
    this->logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

    this->logger.LogMessage(
        ("Client starting on port " + std::to_string(port) + " with host " + host + " with buffer size " +
         std::to_string(bufferSize)).c_str(), ArgoDraft::LogLevel::INFO);

    this->Run();
}

auto Client::CreateSocket() -> void {
    this->clientSocket = clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (this->clientSocket == -1) {
        std::cerr << "Failed to create socket" << std::endl;
        this->logger.LogMessage("Failed to create client socket", ArgoDraft::LogLevel::CRITICAL);
        return;
    }

    this->logger.LogMessage("Client socket created", ArgoDraft::LogLevel::INFO);
}

auto Client::Connect() -> void {
    sockaddr_in serverAddress{};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(this->port);
    serverAddress.sin_addr.s_addr = inet_addr(this->host.c_str());

    if (connect(this->clientSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to connect to server" << std::endl;
        this->logger.LogMessage("Failed to connect to server", ArgoDraft::LogLevel::CRITICAL);
        this->Close();
        return;
    }

    this->logger.LogMessage("Client connected to server", ArgoDraft::LogLevel::INFO);
}

auto Client::Send(const char *message) const -> bool {
    if (const int sent = send(this->clientSocket, message, strlen(message), 0) >= 0; !sent) {
        std::cerr << "Failed to send message" << std::endl;
        this->logger.LogMessage("Client failed to send message", ArgoDraft::LogLevel::CRITICAL);
        return false;
    }

    return true;
}

auto Client::Receive() const -> void {
    char buffer[this->bufferSize] = {0};
    if (recv(this->clientSocket, buffer, sizeof(buffer), 0) < 0) {
        std::cerr << "Client failed to receive message" << std::endl;
        this->logger.LogMessage("Client failed to receive message", ArgoDraft::LogLevel::CRITICAL);
        return;
    }

    this->logger.LogMessage(("Client received message: " + std::string(buffer)).c_str(), ArgoDraft::LogLevel::INFO);
}

auto Client::Close() -> void {
    // if (close(this->clientSocket) < 0) {
    //     std::cerr << "Failed to close client socket" << std::endl;
    //     this->logger.LogMessage("Failed to close client socket", ArgoDraft::LogLevel::CRITICAL);
    //     return;
    // }

    this->clientSocket = 0;
    this->logger.LogMessage("Client socket closed", ArgoDraft::LogLevel::INFO);
}

auto Client::Run() -> void {
#ifdef __WIN32__
    this->logger.LogMessage("Starting Windows Sockets", ArgoDraft::LogLevel::DEBUG);
    constexpr WORD versionWanted = MAKEWORD(1, 1);
    WSADATA wsaData;
    if (const int WASockResult = WSAStartup(versionWanted, &wsaData); WASockResult != 0) {
        std::cerr << "WSAStartup failed with error: " << WASockResult << std::endl;
        this->logger.LogMessage("WSAStartup failed", ArgoDraft::LogLevel::CRITICAL);
        return;
    }
    this->logger.LogMessage("Client Windows Sockets started", ArgoDraft::LogLevel::DEBUG);
#endif

    // Add messages to the queue
    this->messageQueue.emplace("Client Hello!");
    this->messageQueue.emplace("A second message from the client.");
    this->messageQueue.emplace("And a third message from the client.");

    // Add an absurdly long message to the queue
    // this->messageQueue.emplace(
    //     "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras viverra sollicitudin commodo. Integer facilisis"
    //     " fermentum neque, sed congue tellus auctor sit amet. Sed felis massa, faucibus at mollis porttitor, fermentum "
    //     "a nulla. Pellentesque eget fringilla diam. Morbi eget congue purus, vitae facilisis felis. Praesent vitae semper "
    //     "sapien. Aenean egestas lacus mi. Integer faucibus mollis fringilla. Pellentesque felis urna, condimentum eget "
    //     "blandit quis, posuere a quam. Morbi non lacus id metus porta tristique. Praesent vitae dui id elit placerat "
    //     "pharetra eget et magna. Integer nec iaculis arcu. -- Duis et volutpat leo. Sed tempus lacus blandit arcu "
    //     "interdum suscipit. Proin a urna vitae risus vulputate condimentum vitae in dolor. Suspendisse sed malesuada "
    //     "dolor, sed tempor nisl. Quisque luctus neque velit, non dapibus turpis pulvinar eget. Lorem ipsum dolor sit "
    //     "amet, consectetur adipiscing elit. Morbi eu lectus in purus dignissim molestie ac a massa. Phasellus tincidunt, "
    //     "eros ut gravida vehicula, sem nisl elementum nibh, suscipit rhoncus libero fusce."
    // );

    this->messageQueue.emplace("And a final message from the client.");

    this->messageQueue.emplace("//exit");

    this->CreateSocket();

    this->Connect();

    if (this->clientSocket > 0) {
        // while (true) {

        // While loop to send messages
        while (!this->messageQueue.empty()) {
            if (const auto message = this->messageQueue.front().c_str(); this->Send(message)) {
                this->logger.LogMessage(("Client sent message: " + this->messageQueue.front()).c_str(),
                                        ArgoDraft::LogLevel::INFO);

                this->messageQueue.pop();

                if (std::string(message) == "//exit") {
                    this->Close();
                    break;
                }
            }
        }

        // this->Receive();

        // }

        this->Close();
    }

#ifdef __WIN32__
    this->logger.LogMessage("Cleaning up Windows Sockets", ArgoDraft::LogLevel::DEBUG);
    WSACleanup();
#endif
}
