#include "utils/utils.h"

#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>

constexpr int BUF_SIZE = 1024;

void send_and_recv_back(int sockfd, std::string message, char* buf, int buf_size) {
    std::cout << "Client socket: " << sockfd <<" starting to send message to server...." << std::endl;
    
    send(sockfd, message.c_str(), sizeof(message) + 1, 0);                 // +1 for null character

    std::cout << "Client socket: " << sockfd << " send:     " << message <<std::endl;
    
    sleep(1.0);

    int recv_len = recv(sockfd, buf, sizeof(buf), 0);
    
    if (recv_len == -1) {
        print_error_message_and_exit("Receiving message from the server failed...");
    } else if (recv_len != 0) {
        std::cout << "Client socket: " << sockfd << " received: ";

        for (int i = 0; i < recv_len; ++i) {
            std::cout << buf[i];
        }

        std::cout << std::endl;
    }
}

void test_send_message(int sockfd) {
    char buf[BUF_SIZE];
    int recv_len = recv(sockfd, buf, sizeof(buf), 0);

    if (recv_len == -1) {
        print_error_message_and_exit("Receiving '*' from the server failed...");
    } else if (recv_len != 0) {
        std::cout << "Received: " << buf << std::endl;
    }

    send_and_recv_back(sockfd, "^abc$de^abte$f", buf, BUF_SIZE);
    send_and_recv_back(sockfd, "xyz^123", buf, BUF_SIZE);
    send_and_recv_back(sockfd, "25$^ab0000$abab", buf, BUF_SIZE);

    close(sockfd);
}

int main(int args, char** argv) {
    std::thread client1(test_send_message, create_initialize_client_socket(7777));
    std::thread client2(test_send_message, create_initialize_client_socket(8888));
    std::thread client3(test_send_message, create_initialize_client_socket(9999));

    client1.join();
    client2.join();
    client3.join();
    
    sleep(20);
    return 0;
}