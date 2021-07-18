#ifndef UTILS_H
#define UTILS_H
#include <netinet/in.h>
#include <string>

void report_peer_connected(const sockaddr_in* sa, socklen_t salen);

void print_error_message_and_exit(std::string message);

int create_intialize_server_socket(int portNum);
int create_initialize_client_socket(int portNum);

void make_socket_nonblocking(int sockfd);

#endif
