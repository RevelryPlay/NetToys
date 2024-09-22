#include <iostream>
#include <ArgoDraft/ConsoleMenu.hpp>
#include <ArgoDraft/ArgoLogger.hpp>

#include "Client/Client.hpp"
#include "Server/Server.hpp"

int main()
{
    std::cout << "Type `help` for a list of commands" << std::endl;

    try {
        ArgoDraft::MainMenu menu;
        menu.AddAction({"server", "Starts the server", []() {
            Server server;
        }});
        menu.AddAction({"client", "Starts the client", []() {
            Client client;
        }});
        menu.AddAction({"log", "Logs a message", []() {
            std::cout << "Adding a message to the log..." << std::endl;
            ArgoDraft::Logger::LogMessage("A log message would go here", "NetToys.log");
        }});

        menu.Init();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
