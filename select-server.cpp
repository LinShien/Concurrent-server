#include "utils/utils.h"
#include "utils/server.h"

#include <sys/select.h>
#include <unistd.h>
#include <iostream>
#include <assert.h>

// FD_SETSIZE is 1024 on Linux. We just use a natural limit to the number of simultaneous FDs monitored by select()
constexpr int MAXFDs = 1000;

/**
 * Callbacks (on_XXX functions) return this status to the main loop. And the status instructs the loop about
 * the next step for the fd which the callback was invoked. (Event - driven)
 * want_to_read = true means we want to keep monitoring this fd for reading.
 * want_to_write = true means we want to keep monitoring this fd for writing.
 * When both fields are false means this the fd is no longer needed and can be closed.
 */ 
struct fd_status {
    bool want_to_read;
    bool want_to_write;
};

const fd_status FD_STATUS_R{true, false};
const fd_status FD_STATUS_W{false, true};
const fd_status FD_STATUS_RW{true, true};
const fd_status FD_STATUS_NORW{false, false};

constexpr int SENDBUF_SIZE = 1024;

/**
 * Used to represent a client connection between callback calls from the main loop.
 * send_buf contains data the server has to send back to the client.
 * The on_peer_ready_recv function will populate this buffer, and on_peer_ready_send drains it.
 * send_buf_end points to the last valid byte in the buffer, and sendptr at the next byte to send.
 */

struct peer_state {
    ProcessingState state;

    char send_buf[SENDBUF_SIZE];
    int send_buf_end;
    int send_buf_next_idx;
};

/**
 * Each peer is globally identified by the file descriptor it's connected on. As long as the peer is connect,
 * the fd is unique to it. When a peer disconnects, a new peer may connect to this server and get the same fd.
 * on_peer_connected should initialize the state properly to remove any trace of the old peer on the same fd.
 */
peer_state global_fds_state[MAXFDs];

void check_listener_sockfd(int server_sockfd) {
    if (server_sockfd >= FD_SETSIZE) {
        std::string error_message = "Server socket fd ";
        error_message.append(std::to_string(server_sockfd));
        error_message.append(" >= FD_SETSIZE ");
        error_message.append(std::to_string(FD_SETSIZE));

        print_error_message_and_exit(error_message);
    }
}

int accept_peer_build_nonblocking_connection(int server_sockfd, int& max_fdset, sockaddr_in& peer_addr, socklen_t peer_addr_len) {
    int new_sockfd = accept(server_sockfd, (sockaddr*) &peer_addr, &peer_addr_len);

    if (new_sockfd == -1) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            std::cerr << "Accept() returned EAGAIN or EWOULDBLOCK \n";             // could happen due to non-blocking mode
        } else {
            print_error_message_and_exit("Accepting failed...");
        }
        
    } else {
        make_socket_nonblocking(new_sockfd);

        if (new_sockfd > max_fdset) {
            check_listener_sockfd(new_sockfd);

            max_fdset = new_sockfd;
        }
    }

    return new_sockfd;
}

// Callback function for accept() function
fd_status on_peer_connected(int sockfd, const sockaddr_in* peer_addr, socklen_t peer_addr_len) {
    assert(sockfd < MAXFDs);

    report_peer_connected(peer_addr, peer_addr_len);

    // Initialize the peer state and send back a character '*' to the client.
    peer_state* state = &global_fds_state[sockfd];
    state->state = ProcessingState::INITIAL_ACK;

    state->send_buf[0] = '*';
    state->send_buf[1] = '\0';
    state->send_buf_next_idx = 0;
    state->send_buf_end = 2;
    
    return FD_STATUS_W;
}

// Callback function for recv() function, will never block because the recv() is in non-blocking mode
fd_status on_peer_ready_recv(int sockfd) {
    assert(sockfd < MAXFDs);

    peer_state* state = &global_fds_state[sockfd];

    if (state->state == ProcessingState::INITIAL_ACK || state->send_buf_next_idx < state->send_buf_end) {   
        // In send state or INITIAL_ACK state should write to the peer next
        return FD_STATUS_W;
    }

    char buf[1024];

    int received_len =  recv(sockfd, buf, sizeof(buf), 0);

    // polling
    if (received_len == -1) {            
        // The socket is not *really* ready for recv; wait until it is.
        // and the socket is in non-blocking mode and data is not available yet.
        if (errno == EAGAIN || errno == EWOULDBLOCK) {           
            return FD_STATUS_R;
        } else {
            print_error_message_and_exit("Receiving message from the client failed...");
        }

    } else if (received_len == 0) {                                                        // this client disconnected
        return FD_STATUS_NORW;
    }

    // Sucessfully polling and got data sent from peer

    bool ready_to_send = false;

    for (int i = 0; i < received_len; ++i) {
        switch (state->state) {
            case ProcessingState::INITIAL_ACK: 
                assert(0 && "can't reach here(unusal state). No recv() called before sending '*' to peer");

                break;
            
            case ProcessingState::WAIT_FOR_MSG: 
                if (buf[i] == '^') {
                    state->state = ProcessingState::IN_MSG;
                }
                break;
                
            case ProcessingState::IN_MSG: 
                if (buf[i] == '$') {
                    state->state = ProcessingState::WAIT_FOR_MSG;
                } else {
                    assert(state->send_buf_end < SENDBUF_SIZE);        

                    state->send_buf[state->send_buf_end++] = buf[i];

                    ready_to_send = true;
                }

                break;
        }
    }

    // report reading readiness iff there's nothing to send to the peer as a result of the latest recv call.
    return fd_status{!ready_to_send, ready_to_send};
}

// Callback function for send() function, will never block because the send() is in non-blocking mode
fd_status on_peer_ready_send(int sockfd) {
    assert(sockfd < MAXFDs);

    peer_state* state = &global_fds_state[sockfd];

    if (state->send_buf_next_idx >= state->send_buf_end) {          // nothing to send now 
        return FD_STATUS_RW;
    }

    int send_len = state->send_buf_end - state->send_buf_next_idx;
    int sent_len = send(sockfd, &state->send_buf[state->send_buf_next_idx], send_len, 0);          // non-blocking send

    // polling
    if (sent_len == -1) {            
        // The socket is not *really* ready for send; wait until it is.
        // and the socket is in non-blocking mode and data is not available yet.
        // back to FD_STATUS_W state and re-run callback on_peer_ready_recv next loop
        if (errno == EAGAIN || errno == EWOULDBLOCK) {           
            return FD_STATUS_W;                                                                                 
        } else {
            print_error_message_and_exit("Sending message from the client failed...");
        }
    }

    if (sent_len < send_len) {          
        state->send_buf_next_idx += sent_len;                // successfully sent partial message
        return FD_STATUS_W; 
    } else {                                                                                       // successfully sent all message, into next state
        state->send_buf_next_idx = 0;
        state->send_buf_end = 0;

        if (state->state == ProcessingState::INITIAL_ACK) {         // just sent '*' to client and wait for '^' from client
            state->state = ProcessingState::WAIT_FOR_MSG;
        }

        return FD_STATUS_R; 
    }
}

void change_fdset(int sockfd, const fd_status& status, fd_set& readfds, fd_set& writefds) {
    if (status.want_to_read) {
        FD_SET(sockfd, &readfds);
    } else {
        FD_CLR(sockfd, &readfds);
     }

    if (status.want_to_write) {
        FD_SET(sockfd, &writefds);
    } else {
        FD_CLR(sockfd, &writefds);
    }
}

int main(int args, char** argv) {
    setvbuf(stdout, NULL, _IOFBF, 0);

    int portNum = 9090;

    if (args >= 2) {
        portNum = atoi(argv[1]);
    } 

    int server_sockfd = create_intialize_server_socket(portNum);

    make_socket_nonblocking(server_sockfd);

    check_listener_sockfd(server_sockfd);

    // Tracking which FDs we want to monitor for reading(receiving) and which FDs we want to monitor for writing(sending)
    fd_set main_readfds;
    fd_set main_writefds;

    FD_ZERO(&main_readfds);
    FD_ZERO(&main_writefds);

    // The listening socket is always monitored for read, to detect when new peer connections are coming
    FD_SET(server_sockfd, &main_readfds);

    int max_fdset = server_sockfd;

    // event loop
    while (true) {
        // select() will modify the fd_sets passed to it
        fd_set readfds = main_readfds;
        fd_set writefds = main_writefds;

        int nums_ready = select(max_fdset + 1, &readfds, &writefds, nullptr, nullptr);           // blocking API

        if (nums_ready == -1) {
            print_error_message_and_exit("Failed to select() the monitored sockets...");
        }

        for (int fd = 0; fd <= max_fdset && nums_ready > 0; ++fd) {            // iterate all the possible FDs
            // check read events
            if (FD_ISSET(fd, &readfds)) {
                --nums_ready;

                if (fd == server_sockfd) {
                    sockaddr_in peer_addr;
                    socklen_t peer_addr_len = sizeof(peer_addr);

                    // The listening socket is ready, a new peer connection have to be created
                    int new_sockfd = accept_peer_build_nonblocking_connection(server_sockfd, max_fdset, peer_addr, peer_addr_len);

                    fd_status status = on_peer_connected(new_sockfd, &peer_addr, peer_addr_len);
                    change_fdset(new_sockfd, status, main_readfds, main_writefds);

                } else {                                      
                    fd_status status = on_peer_ready_recv(fd);
                    change_fdset(fd, status, main_readfds, main_writefds);

                    if (!status.want_to_read && !status.want_to_write) {
                        std::cout << "socket " << fd << " closing...." << std::endl;
                        close(fd);
                    }  
                }
            } 

            // check write events
            if (FD_ISSET(fd, &writefds)) {
                --nums_ready;

                fd_status status = on_peer_ready_send(fd);
                change_fdset(fd, status, main_readfds, main_writefds);
                
                if (!status.want_to_read && !status.want_to_write) {
                    std::cout << "socket " << fd << " closing...." << std::endl;
                    close(fd);
                }  
            }
        }
    }

    close(server_sockfd);
    return 0;
}