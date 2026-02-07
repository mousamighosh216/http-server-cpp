#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <cstdio>
#include <sys/socket.h>
#include <unistd.h>
#include "handle_client.h"

constexpr int BUFFER_SIZE = 4096; 
constexpr const char* INDEX_FILENAME = "index.html";

struct HttpRequest {
    std::string method;  // GET, POST, etc.
    std::string uri;     // /index.html
    std::string version; // HTTP/1.1
    std::unordered_map<std::string, std::string> headers; // Dynamic headers
    std::string body;    // Optional POST/PUT body
};

struct StatusLine {
    std::string version; // HTTP/1.1
    std::string status;  // 200, 404, etc.
    std::string reason;  // OK, Not Found, etc.
};

struct HttpResponse {
    StatusLine status_line;
    std::string content_type;  // text/html, application/json
    uint32_t content_length;   // length of body in bytes
    std::string body;          // actual content
};

bool parse_http_request(const std::string &msg, HttpRequest &req) {
    std::istringstream stream(msg);
    std::string request_line;
    std::getline(stream, request_line);
    if (!request_line.empty() && request_line.back() == '\r') {
        request_line.pop_back();
    }

    // parse request line
    std::istringstream iss(request_line);
    iss >> req.method >> req.uri >> req.version;
    if (req.method.empty() || req.uri.empty() || req.version.empty()) {
        return 0; // invalid request line
    }
    // parse headers
    std::string header_line;
    while (std::getline(stream, header_line) && !header_line.empty()) {
        if (header_line.back() == '\r') header_line.pop_back();
        auto sep_pos = header_line.find(':');
        if (sep_pos != std::string::npos) {
            std::string key = header_line.substr(0, sep_pos);
            std::string value = header_line.substr(sep_pos + 1);
            // trim spaces from value
            value.erase(0, value.find_first_not_of(" "));
            req.headers[key] = value;
        }
    }
    return 1;
}


std::string create_http_response(const std::string& content, HttpResponse& res) {
    res.status_line.status = "200";
    res.status_line.reason = "OK";
    res.status_line.version = "HTTP/1.1";

    if(content.empty()) {
        res.status_line.status = "500";
        res.status_line.reason = "Server Error";
    }

    res.content_length = content.length();
    res.content_type = "text/html";

    std::ostringstream oss;
    oss << res.status_line.version << " " << res.status_line.status << " " << res.status_line.reason << "\r\n"
        << "Content-Type: " << res.content_type << "\r\n"
        << "Content-Length: " << res.content_length << "\r\n\r\n"
        << content;

    std::string response = oss.str();
    return response;
}

// long get_fsize(FILE* fd) {
//     if (!fd) return -1;
//     long current = ftell(fd);        // save position
//     fseek(fd, 0, SEEK_END);
//     long size = ftell(fd);
//     fseek(fd, current, SEEK_SET);    // restore position
//     return size;
// }


int handle_client(int client_socket) {
    // recv request: you can read incoming data from the remote side using the recv() (for TCP SOCK_STREAM sockets)
    std::vector<char> buf(BUFFER_SIZE);

    int bytes_recv = 0;
    std::string request_data;
    while ((bytes_recv = recv(client_socket, buf.data(), buf.size(), 0)) > 0) {
        request_data.append(buf.data(), bytes_recv);
        if (request_data.find("\r\n\r\n") != std::string::npos) break;
    }
    if (bytes_recv < 0) return -1;
    HttpRequest req;
    if (!parse_http_request(request_data, req)) return -1;
    std::string path;
    if (req.uri == "/")
        path = "../html/index.html";
    else
        path = "../html" + req.uri;
    req.uri = path;
    std::ifstream fd(req.uri, std::ios::binary);
    if (!fd) {
        std::cerr << "error opening file\n";
        return -1;
    }

    // response
    std::string content((std::istreambuf_iterator<char>(fd)), std::istreambuf_iterator<char>());
    HttpResponse res;
    std::string response = create_http_response(content, res);
    if(response.size() < 1) {
        printf("error creating response: %d\n", errno);
        return -1;
    }
    printf("%s\n", response);
    int bytes_sent;
    if((bytes_sent = send(client_socket, response.data(), response.size(), 0)) == -1) return -1;

    printf("Closed client socket\n");
    close(client_socket);
    return 0;
}

