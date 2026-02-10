#include "HttpServer.h"
#include <iostream>
#include <csignal>
#include <cstdlib>

std::unique_ptr<HttpServer> server;

void signalHandler(int signum) {
    std::cout << "\nInterrupt signal (" << signum << ") received.\n";
    if (server) {
        server->stop();
    }
    exit(signum);
}

int main(int argc, char* argv[]) {
    // Регистрируем обработчики сигналов
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    
    int port = 3000;
    if (argc > 1) {
        port = std::atoi(argv[1]);
    }
    
    std::cout << "=== C++ Web Application ===" << std::endl;
    std::cout << "Starting server on port " << port << std::endl;
    std::cout << "Endpoints:" << std::endl;
    std::cout << "  GET /          - Main page" << std::endl;
    std::cout << "  GET /health    - Health check" << std::endl;
    std::cout << "  GET /api/hello - Hello API" << std::endl;
    std::cout << "  GET /api/time  - Current time" << std::endl;
    std::cout << "Press Ctrl+C to stop\n" << std::endl;
    
    try {
        server = std::make_unique<HttpServer>(port);
        server->start();
        
        // Ждем завершения
        while (server->isRunning()) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
