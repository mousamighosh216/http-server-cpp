#include <csignal>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include "sig_utils.h"

void sigchld_handler(int) {
    // Reap all exited children
    while (waitpid(-1, nullptr, WNOHANG) > 0) {}
}

void termination_handler(int) {
    std::cout << "\nShutting down server...\n";
    std::exit(0);
}

int init_signal_handler() {
    struct sigaction sa{};
    sa.sa_flags = SA_RESTART;          // restart interrupted syscalls
    sigemptyset(&sa.sa_mask);

    // SIGCHLD
    sa.sa_handler = sigchld_handler;// Set handler function
    if (sigaction(SIGCHLD, &sa, nullptr) == -1) {
        perror("Error setting up SIGCHLD handler");
        return -1;
    }

    // SIGTERM & SIGINT
    sa.sa_handler = termination_handler;
    if (sigaction(SIGTERM, &sa, nullptr) == -1 ||
        sigaction(SIGINT,  &sa, nullptr) == -1) {
        perror("Error setting up SIGTERM handler");
        return -1;
    }

    return 0;
}
