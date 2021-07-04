#include "utils/utils.h"
#include "utils/server.h"
#include <iostream>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>

void task(int sockfd) {
    std::cout << "Thread " << std::this_thread::get_id() << " created to handle connection with socket " << sockfd << std::endl;
    serve_connection(sockfd);
    std::cout << "Thread " << std::this_thread::get_id() << " done. " << std::endl;
}

int main(int args, char** argv) {
    setvbuf(stdout, NULL, _IOFBF, 0);

    int portNum = 9090;

    if (args >= 2) {
        portNum = atoi(argv[1]);
    } 

    int server_sockfd = create_intialize_server_socket(portNum);

    std::cout << "Serving on port: " << portNum << std::endl;
    std::cout << "Starting to accept any connection...." << std::endl;

    while (true) {
        sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        int new_sockfd = accept(server_sockfd, (sockaddr*) &peer_addr, &peer_addr_len);

        if (new_sockfd == -1) {
           print_error_message_and_exit("Accepting failed...");
        }

        report_peer_connected(&peer_addr, peer_addr_len);

        std::thread thread_handle_connection(task, new_sockfd);
        thread_handle_connection.detach();
    }

    close(server_sockfd);
    return 0;
}