#include <string>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <fstream>
#include <sstream>
#include <unordered_map>

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
    if (req.method.empty() || req.uri.empty() || req.version.empty()) {
        return -1; // invalid request line
    }
    iss >> req.method >> req.uri >> req.version;

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

long get_fsize(const std::string_view filename = INDEX_FILENAME) {
    std::ifstream file(filename.data(), std::ios::binary | std::ios::ate);
    if (!file) return -1;
    auto fsize = file.tellg();
    file.seekg(0, std::ios::beg);
    return fsize;
}

int handle_client(int client_socket) {
    // recv request: you can read incoming data from the remote side using the recv() (for TCP SOCK_STREAM sockets)
    

}

