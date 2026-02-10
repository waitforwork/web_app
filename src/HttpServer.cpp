#include "HttpServer.h"
#include <iostream>
#include <sstream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

HttpServer::HttpServer(int port) : port_(port), running_(false) {
    // Дефолтные роуты
    addRoute("GET", "/", [](const std::string&) {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: text/html\r\n"
               "\r\n"
               "<html><body>"
               "<h1>Hello from C++ Web App!</h1>"
               "<p>Server is running successfully!</p>"
               "<ul>"
               "<li><a href=\"/health\">Health Check</a></li>"
               "<li><a href=\"/api/hello\">API Hello</a></li>"
               "<li><a href=\"/api/time\">Current Time</a></li>"
               "</ul>"
               "</body></html>";
    });
    
    addRoute("GET", "/health", [](const std::string&) {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "\r\n"
               "{\"status\": \"healthy\", \"service\": \"cpp-webapp\"}";
    });
    
    addRoute("GET", "/api/hello", [](const std::string&) {
        return "HTTP/1.1 200 OK\r\n"
               "Content-Type: application/json\r\n"
               "\r\n"
               "{\"message\": \"Hello from C++ API!\", \"language\": \"C++\"}";
    });
    
    addRoute("GET", "/api/time", [](const std::string&) {
        time_t now = time(nullptr);
        std::string time_str = ctime(&now);
        time_str.pop_back(); // Убрать \n
        
        std::ostringstream json;
        json << "HTTP/1.1 200 OK\r\n"
             << "Content-Type: application/json\r\n"
             << "\r\n"
             << "{\"timestamp\": \"" << time_str << "\", "
             << "\"unix_time\": " << now << "}";
        
        return json.str();
    });
}

HttpServer::~HttpServer() {
    stop();
}

void HttpServer::addRoute(const std::string& method, const std::string& path, Handler handler) {
    routes_[method][path] = handler;
}

void HttpServer::start() {
    if (running_) return;
    
    // Создаем сокет
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0) {
        throw std::runtime_error("Failed to create socket");
    }
    
    // Настраиваем опции сокета
    int opt = 1;
    if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Failed to set socket options");
    }
    
    // Настраиваем адрес
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    
    // Биндим сокет
    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) < 0) {
        throw std::runtime_error("Failed to bind socket");
    }
    
    // Начинаем слушать
    if (listen(server_fd_, BACKLOG) < 0) {
        throw std::runtime_error("Failed to listen on socket");
    }
    
    std::cout << "Server starting on port " << port_ << std::endl;
    
    running_ = true;
    server_thread_ = std::thread(&HttpServer::run, this);
}

void HttpServer::stop() {
    if (!running_) return;
    
    running_ = false;
    close(server_fd_);
    
    if (server_thread_.joinable()) {
        server_thread_.join();
    }
    
    std::cout << "Server stopped" << std::endl;
}

void HttpServer::run() {
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    
    while (running_) {
        int client_fd = accept(server_fd_, 
                              (struct sockaddr*)&client_addr, 
                              &client_len);
        
        if (client_fd < 0) {
            if (running_) {
                std::cerr << "Failed to accept connection" << std::endl;
            }
            continue;
        }
        
        // Читаем запрос
        char buffer[BUFFER_SIZE];
        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            std::string request(buffer);
            
            // Обрабатываем запрос
            std::string response = handleRequest(request);
            
            // Отправляем ответ
            write(client_fd, response.c_str(), response.length());
        }
        
        close(client_fd);
    }
}

std::string HttpServer::handleRequest(const std::string& request) {
    std::string method = parseMethod(request);
    std::string path = parsePath(request);
    
    std::cout << "Request: " << method << " " << path << std::endl;
    
    // Ищем обработчик
    auto method_it = routes_.find(method);
    if (method_it != routes_.end()) {
        auto path_it = method_it->second.find(path);
        if (path_it != method_it->second.end()) {
            return path_it->second(request);
        }
    }
    
    // 404 если роут не найден
    return "HTTP/1.1 404 Not Found\r\n"
           "Content-Type: text/html\r\n"
           "\r\n"
           "<html><body><h1>404 Not Found</h1><p>The requested URL was not found on this server.</p></body></html>";
}

std::string HttpServer::parseMethod(const std::string& request) {
    size_t space_pos = request.find(' ');
    if (space_pos != std::string::npos) {
        return request.substr(0, space_pos);
    }
    return "GET";
}

std::string HttpServer::parsePath(const std::string& request) {
    size_t first_space = request.find(' ');
    if (first_space != std::string::npos) {
        size_t second_space = request.find(' ', first_space + 1);
        if (second_space != std::string::npos) {
            return request.substr(first_space + 1, second_space - first_space - 1);
        }
    }
    return "/";
}
