/**
 * @file HttpServer.cpp
 * @brief HTTP 服务器实现
 */

#include "HttpServer.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cctype>
#include <algorithm>

#ifdef _WIN32
static bool wsaInitialized = false;
#endif

namespace {
std::string toLowerAscii(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

size_t parseContentLength(const std::string& headers) {
    std::istringstream stream(headers);
    std::string line;

    while (std::getline(stream, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        size_t colonPos = line.find(':');
        if (colonPos == std::string::npos) {
            continue;
        }

        std::string key = toLowerAscii(line.substr(0, colonPos));
        if (key != "content-length") {
            continue;
        }

        std::string value = line.substr(colonPos + 1);
        while (!value.empty() && value.front() == ' ') {
            value.erase(value.begin());
        }
        try {
            return static_cast<size_t>(std::stoul(value));
        } catch (...) {
            return 0;
        }
    }

    return 0;
}
}  // namespace

HttpServer::HttpServer() {
    initSocket();
}

HttpServer::~HttpServer() {
    stop();
    cleanupSocket();
}

bool HttpServer::initSocket() {
#ifdef _WIN32
    if (!wsaInitialized) {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            return false;
        }
        wsaInitialized = true;
    }
#endif
    return true;
}

void HttpServer::cleanupSocket() {
#ifdef _WIN32
    if (wsaInitialized) {
        WSACleanup();
        wsaInitialized = false;
    }
#endif
}

void HttpServer::route(const std::string& method, const std::string& path,
                        RequestHandler handler) {
    routes_[method + " " + path] = handler;
}

void HttpServer::get(const std::string& path, RequestHandler handler) {
    route("GET", path, handler);
}

void HttpServer::post(const std::string& path, RequestHandler handler) {
    route("POST", path, handler);
}

void HttpServer::setStaticDir(const std::string& dir) {
    staticDir_ = dir;
}

bool HttpServer::start(int port) {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket_ == SOCKET_INVALID) {
        std::cerr << "Failed to create socket" << std::endl;
        return false;
    }

    // 允许端口复用
    int opt = 1;
#ifdef _WIN32
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR,
               (const char*)&opt, sizeof(opt));
#else
    setsockopt(serverSocket_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

    struct sockaddr_in serverAddr;
    std::memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket_, (struct sockaddr*)&serverAddr,
             sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind to port " << port << std::endl;
        CLOSE_SOCKET(serverSocket_);
        return false;
    }

    if (listen(serverSocket_, 10) < 0) {
        std::cerr << "Failed to listen" << std::endl;
        CLOSE_SOCKET(serverSocket_);
        return false;
    }

    running_ = true;
    serverThread_ = std::thread(&HttpServer::serverLoop, this);

    std::cout << "Server started at http://localhost:" << port << std::endl;
    return true;
}

void HttpServer::stop() {
    running_ = false;
    if (serverSocket_ != SOCKET_INVALID) {
        CLOSE_SOCKET(serverSocket_);
        serverSocket_ = SOCKET_INVALID;
    }
    if (serverThread_.joinable()) {
        serverThread_.join();
    }
}

bool HttpServer::isRunning() const {
    return running_;
}

void HttpServer::serverLoop() {
    while (running_) {
        struct sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        SocketType clientSocket = accept(serverSocket_,
            (struct sockaddr*)&clientAddr, &clientLen);

        if (clientSocket == SOCKET_INVALID) {
            if (running_) {
                std::cerr << "Accept failed" << std::endl;
            }
            continue;
        }

        // 处理客户端请求（简单实现，单线程处理）
        handleClient(clientSocket);
        CLOSE_SOCKET(clientSocket);
    }
}

void HttpServer::handleClient(SocketType clientSocket) {
    constexpr size_t kBufferSize = 8192;
    constexpr size_t kMaxRequestSize = 50 * 1024 * 1024;  // 50MB
    char buffer[kBufferSize];
    std::string rawRequest;
    rawRequest.reserve(kBufferSize);

    size_t expectedTotalSize = 0;
    bool headerParsed = false;

    while (true) {
        std::memset(buffer, 0, sizeof(buffer));

#ifdef _WIN32
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
#else
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
#endif

        if (bytesRead <= 0) {
            break;
        }

        rawRequest.append(buffer, static_cast<size_t>(bytesRead));

        if (rawRequest.size() > kMaxRequestSize) {
            return;
        }

        if (!headerParsed) {
            size_t headerEnd = rawRequest.find("\r\n\r\n");
            if (headerEnd != std::string::npos) {
                headerParsed = true;
                size_t contentLength = parseContentLength(rawRequest.substr(0, headerEnd));
                expectedTotalSize = headerEnd + 4 + contentLength;

                if (contentLength == 0 || rawRequest.size() >= expectedTotalSize) {
                    break;
                }
            }
        } else if (rawRequest.size() >= expectedTotalSize) {
            break;
        }
    }

    if (rawRequest.empty()) {
        return;
    }

    if (expectedTotalSize > 0 && rawRequest.size() > expectedTotalSize) {
        rawRequest.resize(expectedTotalSize);
    }

    HttpRequest request = parseRequest(rawRequest);
    HttpResponse response;

    // 查找路由
    std::string routeKey = request.method + " " + request.path;
    auto it = routes_.find(routeKey);

    if (it != routes_.end()) {
        it->second(request, response);
    } else if (!staticDir_.empty() && request.method == "GET") {
        // 尝试提供静态文件
        if (!serveStaticFile(request.path, response)) {
            response.setNotFound();
        }
    } else {
        response.setNotFound();
    }

    std::string responseStr = buildResponse(response);
    send(clientSocket, responseStr.c_str(), responseStr.length(), 0);
}

HttpRequest HttpServer::parseRequest(const std::string& raw) {
    HttpRequest request;
    std::istringstream stream(raw);
    std::string line;

    // 解析请求行
    if (std::getline(stream, line)) {
        // 移除 \r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        std::istringstream lineStream(line);
        lineStream >> request.method >> request.path;

        // 解析查询字符串
        size_t queryPos = request.path.find('?');
        if (queryPos != std::string::npos) {
            request.query = request.path.substr(queryPos + 1);
            request.path = request.path.substr(0, queryPos);

            // 解析参数
            std::istringstream queryStream(request.query);
            std::string param;
            while (std::getline(queryStream, param, '&')) {
                size_t eqPos = param.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = urlDecode(param.substr(0, eqPos));
                    std::string value = urlDecode(param.substr(eqPos + 1));
                    request.params[key] = value;
                }
            }
        }
    }

    // 解析头部
    while (std::getline(stream, line) && line != "\r" && !line.empty()) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = line.substr(0, colonPos);
            std::string value = line.substr(colonPos + 1);
            // 去除前导空格
            while (!value.empty() && value[0] == ' ') {
                value = value.substr(1);
            }
            request.headers[key] = value;
        }
    }

    // 读取 body
    std::ostringstream bodyStream;
    bodyStream << stream.rdbuf();
    request.body = bodyStream.str();

    return request;
}

std::string HttpServer::buildResponse(const HttpResponse& response) {
    std::ostringstream stream;

    stream << "HTTP/1.1 " << response.statusCode << " "
           << response.statusText << "\r\n";

    for (const auto& header : response.headers) {
        stream << header.first << ": " << header.second << "\r\n";
    }

    stream << "Content-Length: " << response.body.length() << "\r\n";
    stream << "Connection: close\r\n";
    stream << "\r\n";
    stream << response.body;

    return stream.str();
}

std::string HttpServer::urlDecode(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            int value;
            std::istringstream iss(str.substr(i + 1, 2));
            if (iss >> std::hex >> value) {
                result += static_cast<char>(value);
                i += 2;
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' ';
        } else {
            result += str[i];
        }
    }
    return result;
}

std::string HttpServer::getMimeType(const std::string& path) {
    size_t dotPos = path.rfind('.');
    if (dotPos == std::string::npos) {
        return "application/octet-stream";
    }

    std::string ext = path.substr(dotPos);
    if (ext == ".html" || ext == ".htm") return "text/html; charset=utf-8";
    if (ext == ".css") return "text/css; charset=utf-8";
    if (ext == ".js") return "application/javascript; charset=utf-8";
    if (ext == ".json") return "application/json; charset=utf-8";
    if (ext == ".png") return "image/png";
    if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
    if (ext == ".gif") return "image/gif";
    if (ext == ".svg") return "image/svg+xml";
    if (ext == ".ico") return "image/x-icon";

    return "application/octet-stream";
}

bool HttpServer::serveStaticFile(const std::string& path, HttpResponse& response) {
    std::string filePath = staticDir_;
    if (path == "/") {
        filePath += "/index.html";
    } else {
        filePath += path;
    }

    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }

    std::ostringstream content;
    content << file.rdbuf();
    file.close();

    response.statusCode = 200;
    response.statusText = "OK";
    response.headers["Content-Type"] = getMimeType(filePath);
    response.body = content.str();

    return true;
}
