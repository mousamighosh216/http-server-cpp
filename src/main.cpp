#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include "sig_utils.h"
#include "network.h"
#include "handle_client.h"

int server_socket_fd;

int main() {
    // create socket
    if ((server_socket_fd = create_server_socket()) == -1) {
        return 1;
    }
    // setup signal
    if (init_signal_handler() != 0) {
        return 1;
    }

    // accept incoming connections
    struct sockaddr_storage client_addr{};

    while(1) {
        socklen_t addr_size = sizeof(client_addr);
        int new_client_socket_fd = accept(server_socket_fd, (struct sockaddr *) &client_addr, &addr_size);

        if (new_client_socket_fd == -1) {
            if (errno == EINTR) {
                // If accept() was interrupted by a signal, retry
                continue;
            }
        perror("accept");
        continue;
        }

        print_client_ip(new_client_socket_fd);

        // handle connection in child process
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            close(new_client_socket_fd);
            continue;
        }

        if (pid == 0) {  // child
            close(server_socket_fd);
            int rc = handle_client(new_client_socket_fd);
            close(new_client_socket_fd);   // important
            _exit(rc == -1 ? 1 : 0);       // use _exit after fork
        }

        // parent
        close(new_client_socket_fd);
    }
  return 0;
}
