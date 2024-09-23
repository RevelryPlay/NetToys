#include <iostream>
#include <ArgoDraft/ConsoleMenu.hpp>
#include <ArgoDraft/ArgoLogger.hpp>

#include "Client/Client.hpp"
#include "Server/Server.hpp"

using namespace ArgoDraft;

int main() {
    std::cout << "Type `help` for a list of commands" << std::endl;

    try {
        MainMenu menu;
        menu.AddAction({
            "server", "Starts the server", []() {
                Server server;
            }
        });
        menu.AddAction({
            "client", "Starts the client", []() {
                Client client;
            }
        });
        menu.AddAction({
            "log", "Logs a message", []() {
                std::cout << "Adding a message to the log..." << std::endl;

                const std::string fileName = "NetToys.log";
                auto logger = Logger(fileName);

                logger.SetFileLogLevel(ArgoDraft::LogLevel::WARNING);
                logger.SetConsoleLogLevel(ArgoDraft::LogLevel::INFO);

                logger.LogMessage("A log message would go here", LogLevel::INFO);
            }
        });

        menu.Init();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
