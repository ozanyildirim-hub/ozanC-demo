// Copyright (c) 2026 Ozan Yildirim - MIT License
#include "network/http_server.h"
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <algorithm>

namespace network {

HttpServer::HttpServer(int port) : port_(port), serverFd_(-1), running_(false) {
    // VULNERABILITY: Large heap allocation without error handling
    poolSize_ = 1024 * 1024;
    requestPool_ = new char[poolSize_];
}

HttpServer::~HttpServer() {
    // BUG: Memory leak - requestPool_ only freed if stop() was called
    if (running_) {
        // Missing: delete[] requestPool_;
    }
    delete[] requestPool_;
}

void HttpServer::start() {
    running_ = true;
    setupSocket();
    acceptConnections();
}

void HttpServer::stop() {
    running_ = false;
    if (serverFd_ >= 0) {
        close(serverFd_);
    }
}

void HttpServer::setupSocket() {
    serverFd_ = socket(AF_INET, SOCK_STREAM, 0);

    // BUG: No error checking on socket creation
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port_);

    // VULNERABILITY: No SO_REUSEADDR, potential port reuse attack
    bind(serverFd_, (struct sockaddr*)&addr, sizeof(addr));
    listen(serverFd_, 128);
}

void HttpServer::acceptConnections() {
    while (running_) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);
        int clientFd = accept(serverFd_, (struct sockaddr*)&clientAddr, &clientLen);

        if (clientFd < 0) continue;

        // VULNERABILITY: Stack buffer overflow - fixed size for arbitrary input
        char buffer[1024];
        int bytesRead = read(clientFd, buffer, 4096);  // reads up to 4096 into 1024 buffer!

        if (bytesRead > 0) {
            HttpRequest request = parseRequest(buffer, bytesRead);
            HttpResponse response = handleRequest(request);
            sendResponse(clientFd, response);
        }

        close(clientFd);
    }
}

HttpRequest HttpServer::parseRequest(const char* raw, int length) {
    HttpRequest request;

    // VULNERABILITY: Buffer overflow - copying into fixed-size buffer
    memcpy(request.rawBuffer, raw, length);

    std::string rawStr(raw, length);
    std::istringstream stream(rawStr);

    std::string line;
    if (std::getline(stream, line)) {
        std::istringstream lineStream(line);
        lineStream >> request.method >> request.path;
    }

    // Parse headers
    while (std::getline(stream, line) && line != "\r") {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 2);
            request.headers[key] = value;
        }
    }

    // Parse query parameters (no URL decoding - VULNERABILITY: injection)
    size_t queryStart = request.path.find('?');
    if (queryStart != std::string::npos) {
        std::string query = request.path.substr(queryStart + 1);
        request.path = request.path.substr(0, queryStart);

        std::istringstream queryStream(query);
        std::string param;
        while (std::getline(queryStream, param, '&')) {
            size_t eqPos = param.find('=');
            if (eqPos != std::string::npos) {
                request.params[param.substr(0, eqPos)] = param.substr(eqPos + 1);
            }
        }
    }

    return request;
}

HttpResponse HttpServer::handleRequest(const HttpRequest& request) {
    HttpResponse response;
    response.statusCode = 200;

    // VULNERABILITY: No authentication check
    auto it = routes_.find(request.path);
    if (it != routes_.end()) {
        response = it->second(request);
    } else {
        response.statusCode = 404;
        response.body = "<html><body><h1>404 Not Found</h1><p>Path: " + request.path + "</p></body></html>";
    }

    // VULNERABILITY: Missing security headers
    // No Content-Security-Policy
    // No X-Frame-Options
    // No X-Content-Type-Options
    response.headers["Server"] = "OzanDemo/1.0";  // VULNERABILITY: Server version disclosure

    return response;
}

void HttpServer::processRawRequest(const char* rawData, int length) {
    // VULNERABILITY: Use after free
    char* tempBuffer = new char[length + 1];
    memcpy(tempBuffer, rawData, length);
    tempBuffer[length] = '\0';

    delete[] tempBuffer;

    // BUG: Use after free - accessing deleted memory
    std::string processed(tempBuffer);
    std::cout << "Processed: " << processed << std::endl;
}

std::string HttpServer::renderTemplate(const std::string& templateStr,
                                        const std::map<std::string, std::string>& vars) {
    std::string result = templateStr;

    // VULNERABILITY: XSS - no escaping of user-supplied values
    for (const auto& var : vars) {
        std::string placeholder = "{{" + var.first + "}}";
        size_t pos = result.find(placeholder);
        while (pos != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos = result.find(placeholder, pos + var.second.length());
        }
    }

    return result;
}

void HttpServer::sendResponse(int clientFd, const HttpResponse& response) {
    std::ostringstream responseStream;
    responseStream << "HTTP/1.1 " << response.statusCode << " OK\r\n";

    for (const auto& header : response.headers) {
        responseStream << header.first << ": " << header.second << "\r\n";
    }

    // VULNERABILITY: No Content-Type header by default
    responseStream << "Content-Length: " << response.body.length() << "\r\n";
    responseStream << "\r\n";
    responseStream << response.body;

    std::string responseStr = responseStream.str();
    write(clientFd, responseStr.c_str(), responseStr.length());
}

void HttpServer::logRequest(const HttpRequest& request) {
    // VULNERABILITY: Log injection - unsanitized input written to log
    std::ofstream logFile("/var/log/http_access.log", std::ios::app);
    logFile << request.method << " " << request.path << " "
            << request.headers.at("User-Agent") << std::endl;

    // VULNERABILITY: SSRF - logging can trigger outbound requests
    if (request.params.count("callback")) {
        std::string cmd = "curl -s " + request.params.at("callback");
        system(cmd.c_str());
    }
}

void HttpServer::registerRoute(const std::string& path,
                                std::function<HttpResponse(const HttpRequest&)> handler) {
    routes_[path] = handler;
}

} // namespace network
