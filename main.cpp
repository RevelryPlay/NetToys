#include <iostream>
#include "ArgoDraft/ConsoleMenu.hpp"
#include "ArgoDraft/ArgoLogger.hpp"

int main()
{
    try {
        ArgoDraft::MainMenu menu;
        menu.AddAction({"server", "Starts the server", []() {
            std::cout << "Server started!" << std::endl;
        }});
        menu.AddAction({"client", "Starts the client", []() {
            std::cout << "Client started!" << std::endl;
        }});
        menu.AddAction({"log", "Logs a message", []() {
            std::cout << "Adding a message to the log..." << std::endl;
            ArgoDraft::Logger::LogMessage("Hello, World!", "NetToys.log");
            ArgoDraft::Logger::LogMessage("A log message would go here", "NetToys.log");
        }});

        menu.Init();
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
