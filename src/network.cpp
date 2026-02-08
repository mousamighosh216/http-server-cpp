#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include "network.h"

constexpr int QUEUE_SIZE = 20; 
constexpr const char* MY_PORT = "6969";

/**
* addr_to_str - Convert a socket address to a human-readable string
* @addr: Pointer to a sockeraddr struct, which may be IPv4 or IPv6
* @res: Pointer to output buffer to store the resulting IP address
*
* This function detects whether the provided socket address is IPv4 or IPv6,
* extracts the corresponding IP address, and converts it to a human-readable
* string using inet_ntop(). The result is stored in the buffer pointed to by res.
*
* Note:
* - The res buffer must be large enough to hold an IPv6 string (at least INET6_ADDRSTRLEN).
* - The function does not return a value; the result is written directly to res.
*/
void addr_to_str(struct sockaddr *addr, char* res) {
    const void* res_addr;
    if(addr->sa_family == AF_INET) {
        res_addr = &reinterpret_cast<struct sockaddr_in*>(addr)->sin_addr;
    } else {
        res_addr = &reinterpret_cast<struct sockaddr_in6*>(addr)->sin6_addr;
    }
    inet_ntop(addr->sa_family, res_addr, res, INET6_ADDRSTRLEN);
}

/**
* print_client_ip - Prints the connected client's IP to STDOUT
* @client_socket_fd: File descriptor for the client's socket
*/
void print_client_ip(int client_socket_fd) {
    sockaddr_storage peer_addr{};
    socklen_t len = sizeof(peer_addr);
    getpeername(client_socket_fd, reinterpret_cast<sockaddr*>(&peer_addr), &len);
    char ip[INET6_ADDRSTRLEN];
    addr_to_str(reinterpret_cast<sockaddr*>(&peer_addr), ip);
    std::cout << "server: got connection from " << ip << "\n";
}

/**
* create_server_socket - Resolves local IP address, creates a server socket, binds them, listens on port
*
* Return: a file descriptor representing the server socket
*/
int create_server_socket() {
    addrinfo hints{}, *servinfo;
    // will point to results - servinfo
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    // get ip address and port numbers for host server
    int status = getaddrinfo(nullptr, MY_PORT, &hints, &servinfo);
    if (status != 0) {
        std::cerr << "getaddrinfo: " << gai_strerror(status) << "\n";
        return -1;
    }
    // make a socket, bind it, listen on it
    int sockfd = socket(servinfo->ai_family,
                        servinfo->ai_socktype,
                        servinfo->ai_protocol);

    if (sockfd == -1) return -1;

    // allows us to reuse ip address and port 
    int yes;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        printf("setsockopt: error: %d", errno);
        return -1;
    } 
    
    if (bind(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
        printf("bind: error");
        return -1;
    }

    if (listen(sockfd, QUEUE_SIZE) == -1) {
        printf("listen: error");
        return -1;
    }

    char ip[INET6_ADDRSTRLEN];
    addr_to_str(servinfo->ai_addr, ip);
    std::cout << "Listening on " << ip << ":" << MY_PORT << "\n";
    freeaddrinfo(servinfo);
    return sockfd;
}