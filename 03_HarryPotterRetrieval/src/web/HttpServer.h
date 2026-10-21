/**
 * @file HttpServer.h
 * @brief 简单的跨平台 HTTP 服务器
 *
 * 支持平台：Linux, macOS, Windows
 * 无外部依赖，仅使用标准库和系统 socket API
 */

#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <string>
#include <map>
#include <functional>
#include <thread>
#include <atomic>

// 跨平台 socket 头文件
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    typedef SOCKET SocketType;
    #define SOCKET_INVALID INVALID_SOCKET
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    typedef int SocketType;
    #define SOCKET_INVALID -1
    #define CLOSE_SOCKET close
#endif

/**
 * @struct HttpRequest
 * @brief HTTP 请求
 */
struct HttpRequest {
    std::string method;
    std::string path;
    std::string query;
    std::map<std::string, std::string> headers;
    std::map<std::string, std::string> params;
    std::string body;
};

/**
 * @struct HttpResponse
 * @brief HTTP 响应
 */
struct HttpResponse {
    int statusCode = 200;
    std::string statusText = "OK";
    std::map<std::string, std::string> headers;
    std::string body;

    void setContentType(const std::string& type) {
        headers["Content-Type"] = type;
    }

    void setJson(const std::string& json) {
        setContentType("application/json; charset=utf-8");
        body = json;
    }

    void setHtml(const std::string& html) {
        setContentType("text/html; charset=utf-8");
        body = html;
    }

    void setNotFound() {
        statusCode = 404;
        statusText = "Not Found";
        setHtml("<h1>404 Not Found</h1>");
    }
};

// 请求处理函数类型
using RequestHandler = std::function<void(const HttpRequest&, HttpResponse&)>;

/**
 * @class HttpServer
 * @brief 简单的 HTTP 服务器
 */
class HttpServer {
public:
    HttpServer();
    ~HttpServer();

    /**
     * @brief 注册路由处理器
     * @param method HTTP 方法 (GET, POST, etc.)
     * @param path 路径
     * @param handler 处理函数
     */
    void route(const std::string& method, const std::string& path,
               RequestHandler handler);

    /**
     * @brief GET 路由快捷方法
     */
    void get(const std::string& path, RequestHandler handler);

    /**
     * @brief POST 路由快捷方法
     */
    void post(const std::string& path, RequestHandler handler);

    /**
     * @brief 设置静态文件目录
     */
    void setStaticDir(const std::string& dir);

    /**
     * @brief 启动服务器
     * @param port 端口号
     * @return true 如果成功启动
     */
    bool start(int port);

    /**
     * @brief 停止服务器
     */
    void stop();

    /**
     * @brief 检查服务器是否运行中
     */
    bool isRunning() const;

private:
    SocketType serverSocket_ = SOCKET_INVALID;
    std::atomic<bool> running_{false};
    std::thread serverThread_;
    std::map<std::string, RequestHandler> routes_;
    std::string staticDir_;

    void serverLoop();
    void handleClient(SocketType clientSocket);
    HttpRequest parseRequest(const std::string& raw);
    std::string buildResponse(const HttpResponse& response);
    std::string urlDecode(const std::string& str);
    std::string getMimeType(const std::string& path);
    bool serveStaticFile(const std::string& path, HttpResponse& response);

    static bool initSocket();
    static void cleanupSocket();
};

#endif // HTTPSERVER_H
