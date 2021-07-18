#include "utils.h"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string>
#include <fcntl.h>

const int N_BACKLOG = 64;

void report_peer_connected(const sockaddr_in* sa, socklen_t salen) {
    char host_buf[NI_MAXHOST];
    char service_buf[NI_MAXSERV];

    if (getnameinfo((sockaddr*) sa, salen, host_buf, sizeof(host_buf), service_buf, sizeof(service_buf), 0) == 0) {
        std::cout << "peer" << "(" << host_buf << "," << service_buf << ")" << std::endl;
    } else {
        std::cout << "peer (unknown) connected" << std::endl;
    }
}

void print_error_message_and_exit(std::string message) {
    std::cerr << "message" << std::endl;
    exit(1);
}

int create_intialize_server_socket(int portNum) {
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
    
    if (server_sockfd == -1) {
        print_error_message_and_exit("Creating a socket failed... :");
    }

    int opt = 1;

    // 一個 port 釋放後會等待兩分鐘之後才能再被使用，SO_REUSEADDR 是讓 port 釋放後立即就可以被再次使用
    if (setsockopt(server_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) == -1) {
        print_error_message_and_exit("Setting options of a socket failed...");
    }

    sockaddr_in service_addr;
    service_addr.sin_family = AF_INET;
    service_addr.sin_addr.s_addr = INADDR_ANY;
    service_addr.sin_port = htons(portNum);

    if (bind(server_sockfd, (sockaddr*) &service_addr, sizeof(service_addr)) == -1) {
        print_error_message_and_exit("Binding a socket failed...");
    }
    
    if (listen(server_sockfd, N_BACKLOG) == -1) {
        print_error_message_and_exit("Listening failed...");
    }

    std::cout << "Sucessfully created a server socket" << std::endl;
    return server_sockfd;
}

int create_initialize_client_socket(int portNum = -1) {
    int cli_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (cli_sockfd == -1) {
        print_error_message_and_exit("Creating a socket failed... :");
    }

    int opt = 1;

    // 一個 port 釋放後會等待兩分鐘之後才能再被使用，SO_REUSEADDR 是讓 port 釋放後立即就可以被再次使用
    if (setsockopt(cli_sockfd, SOL_SOCKET, SO_REUSEADDR, (char*) &opt, sizeof(opt)) == -1) {
        print_error_message_and_exit("Setting options of a socket failed...");
    }

    if (portNum != -1) {
        sockaddr_in cli_addr;
        cli_addr.sin_family = AF_INET;
        inet_pton(AF_INET, "127.0.0.1", &cli_addr.sin_addr) ;
        cli_addr.sin_port = htons(portNum);

        if (bind(cli_sockfd, (sockaddr*) &cli_addr, sizeof(cli_addr)) == -1) {
            print_error_message_and_exit("Binding a socket failed...");
        }
    }
    
    std::cout << "Created a client on port: " << portNum << std::endl;

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(9090);

    std::string addr = "127.0.0.1";

    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) == -1) {
        print_error_message_and_exit("Invalid address/ Address not supported: " + addr);
    }

    if (connect(cli_sockfd, (sockaddr*) &server_addr, sizeof(server_addr)) == -1) {
        print_error_message_and_exit("Connecting to the server failed...");
    }

    return cli_sockfd;
}

void make_socket_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);

    if (flags == -1) {
        print_error_message_and_exit("fcnt F_GETFL failed...");
    }

    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        print_error_message_and_exit("fcnt F_SETFL  O_NONBLOCK failed...");
    }
}