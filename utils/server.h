#ifndef _SERVER_H_
#define _SERVER_H_

enum class ProcessingState {
    WAIT_FOR_MSG,
    IN_MSG
};

void serve_connection(int sockfd);

#endif