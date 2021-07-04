#include "server.h"
#include "utils.h"
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

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