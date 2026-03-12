// Copyright (c) 2026 Ozan Yildirim - MIT License
#pragma once

#include <string>
#include <map>
#include <functional>

namespace network {

struct HttpRequest {
    std::string method;
    std::string path;
    std::string body;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    char rawBuffer[1024];
};

struct HttpResponse {
    int statusCode;
    std::string body;
    std::map<std::string, std::string> headers;
};

class HttpServer {
public:
    HttpServer(int port);
    ~HttpServer();

    void start();
    void stop();
    void registerRoute(const std::string& path, std::function<HttpResponse(const HttpRequest&)> handler);
    HttpResponse handleRequest(const HttpRequest& request);
    void processRawRequest(const char* rawData, int length);
    std::string renderTemplate(const std::string& templateStr, const std::map<std::string, std::string>& vars);
    void sendResponse(int clientFd, const HttpResponse& response);
    void logRequest(const HttpRequest& request);

private:
    int port_;
    int serverFd_;
    bool running_;
    std::map<std::string, std::function<HttpResponse(const HttpRequest&)>> routes_;
    char* requestPool_;
    int poolSize_;
    void setupSocket();
    void acceptConnections();
    HttpRequest parseRequest(const char* raw, int length);
};

} // namespace network
