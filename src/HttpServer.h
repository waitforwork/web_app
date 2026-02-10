#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <functional>
#include <map>
#include <thread>
#include <vector>
#include <atomic>

class HttpServer {
public:
    using Handler = std::function<std::string(const std::string&)>;
    
    HttpServer(int port = 3000);
    ~HttpServer();
    
    void start();
    void stop();
    void addRoute(const std::string& method, const std::string& path, Handler handler);
    
    int getPort() const { return port_; }
    bool isRunning() const { return running_; }
    
private:
    void run();
    std::string handleRequest(const std::string& request);
    std::string parsePath(const std::string& request);
    std::string parseMethod(const std::string& request);
    
    int port_;
    int server_fd_;
    std::atomic<bool> running_;
    std::thread server_thread_;
    
    std::map<std::string, std::map<std::string, Handler>> routes_;
    
    // Константы
    static const int BUFFER_SIZE = 4096;
    static const int BACKLOG = 10;
};

#endif // HTTPSERVER_H
