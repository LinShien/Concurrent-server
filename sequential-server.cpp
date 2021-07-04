#include "utils/utils.h"
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

enum class ProcessingState {
    WAIT_FOR_MSG,
    IN_MSG
};

void serve_connection(int sockfd) {
    if (send(sockfd, "*", 1, 0) == -1) {
        print_error_message_and_exit("Sending '*' failed...");
    }

    ProcessingState state = ProcessingState::WAIT_FOR_MSG;

    // std::cout << "Into Wait for message state" << std::endl;

    while (true) {
        char buf[1024];

        int received_len =  recv(sockfd, buf, sizeof(buf), 0);

        if (received_len == -1) {
            print_error_message_and_exit("Receiving message from the client failed...");
        } else if (received_len == 0) {                               // this client disconnected
            break;
        }

        std::cout << "Received: " << buf << std::endl;

        for (int i = 0; i < received_len; ++i) {
            switch (state) {
                case ProcessingState::WAIT_FOR_MSG : {
                    if (buf[i] == '^') {
                        state = ProcessingState::IN_MSG;
                        // std::cout << "Into In message state" << std::endl;
                    }
                }

                case ProcessingState::IN_MSG : {
                    if (buf[i] == '$') {
                        state = ProcessingState::WAIT_FOR_MSG;
                        // std::cout << "Into Wait for message state" << std::endl;

                    } else {
                        buf[i] += 1;

                        if (send(sockfd, &buf[i], 1, 0) == -1) {
                            std::string error_message = "Sending received char '" + buf[i];
                            error_message += "' failed...";
                            print_error_message_and_exit(error_message);

                            close(sockfd);
                            return;
                        } 
                    }
                    break;
                }
            }
        }
    }

    close(sockfd);
}

int main(int argc, char** argv) {
    setvbuf(stdout, NULL, _IOFBF, 0);

    int portNum = 9090;

    if (argc >= 2) {
        portNum = atoi(argv[1]);
    } 

    int server_sockfd = create_intialize_server_socket(portNum);

    std::cout << "Serving on port: " << portNum << std::endl;
    std::cout << "Starting to accept any connection...." << std::endl;

    while (1) {
        sockaddr_in peer_addr;
        socklen_t peer_addr_len = sizeof(peer_addr);

        int new_sockfd = accept(server_sockfd, (sockaddr*) &peer_addr, &peer_addr_len);

        if (new_sockfd == -1) {
           print_error_message_and_exit("Accepting failed...");
        }

        report_peer_connected(&peer_addr, peer_addr_len);
        serve_connection(new_sockfd);

        std::cout << "Peer done" << std::endl;
    }
    
    close(server_sockfd);
    return 0;
}
 