#include <iostream>
#include <cassert>
#include "../src/HttpServer.h"

void testHttpServer() {
    std::cout << "Running HTTP server tests..." << std::endl;
    
    HttpServer server(9999); // Используем тестовый порт
    
    // Тест: сервер создан
    assert(server.getPort() == 9999);
    assert(!server.isRunning());
    
    // Можно добавить больше тестов здесь
    
    std::cout << "All tests passed!" << std::endl;
}

int main() {
    testHttpServer();
    return 0;
}
