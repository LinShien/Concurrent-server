#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "utils/utils.h"
#include <unistd.h>
#include <thread>

void test_send_message(int sockfd) {
    char buf[1024];
    int recv_len = recv(sockfd, buf, sizeof(buf), 0);

    if (recv_len == -1) {
        print_error_message_and_exit("Receiving '*' from the server failed...");
    } else if (recv_len != 0) {
        std::cout << "Received: " << buf << std::endl;
    }

    std::cout << "Client socket: " << sockfd <<"Starting to send message to server...." << std::endl;

    std::string message = "^abc$de^abte$f";
    send(sockfd, message.c_str(), sizeof(message), 0);
    sleep(1.0);
    recv_len = recv(sockfd, buf, sizeof(buf), 0);
    
    if (recv_len == -1) {
        print_error_message_and_exit("Receiving message from the server failed...");
    } else if (recv_len != 0) {
        std::cout << "Received: " << buf << std::endl;
    }

    message = "xyz^123";
    send(sockfd, message.c_str(), sizeof(message), 0);
    sleep(1.0);
    recv_len = recv(sockfd, buf, sizeof(buf), 0);

    if (recv_len == -1) {
        print_error_message_and_exit("Receiving message from the server failed...");
    } else if (recv_len != 0) {
        std::cout << "Received: " << buf << std::endl;
    }

    message = "25$^ab0000$abab";
    send(sockfd, message.c_str(), sizeof(message), 0);
    sleep(1.0);
    recv_len = recv(sockfd, buf, sizeof(buf), 0);
    
    if (recv_len == -1) {
        print_error_message_and_exit("Receiving message from the server failed...");
    } else if (recv_len != 0) {
        std::cout << "Received: " << buf << std::endl;
    }

    close(sockfd);
}

int main(int args, char** argv) {
    std::thread client1(test_send_message, create_initialize_client_socket(7777));
    std::thread client2(test_send_message, create_initialize_client_socket(8888));
    std::thread client3(test_send_message, create_initialize_client_socket(9999));

    client1.join();
    client2.join();
    client3.join();
    
    return 0;
}