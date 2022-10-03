#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/host.h"
#include "../include/executecommand.h"
#include <sys/select.h>

#define STDIN 0

int yes = 1;
struct host host;
void server__init(struct host *myhost, struct host *new_client, struct host* clients) {
    int sock = 0; // to get the socket result
    int error; // to check if connection is successful
    struct addrinfo hints, * localhost_ai, * temp_ai;
    memset( & hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    error = getaddrinfo(NULL, myhost -> port_num, & hints, & localhost_ai);
    if (error != 0) {
        exit(EXIT_FAILURE);
    }
    for (temp_ai = localhost_ai; temp_ai != NULL; temp_ai = temp_ai -> ai_next) {
        //creating a socket
        sock = socket(temp_ai -> ai_family, temp_ai -> ai_socktype, temp_ai -> ai_protocol);
        if (sock < 0) {
            continue;
        }
        setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, & yes, sizeof(int));//setting the socket options
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, & yes, sizeof(int));
        //bind the socket
        if (bind(sock, temp_ai -> ai_addr, temp_ai -> ai_addrlen) < 0) {
            close(sock);
            continue;
        }
        break;
    }

    // exit if could not bind
    if (temp_ai == NULL) {
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(sock, 10) == -1) {
        exit(EXIT_FAILURE);
    }

    // assign listener to myhost fd
    myhost -> fd = sock;

    freeaddrinfo(localhost_ai);

    // Now we have a listener_fd. We add it to he master list of fds along with stdin.
    fd_set master; // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()
    FD_ZERO( & master); // clear the master and temp sets
    FD_ZERO( & read_fds);
    FD_SET(sock, & master); // Add sock to the master list
    FD_SET(STDIN, & master); // Add STDIN to the master list
    int fdmax = sock > STDIN ? sock : STDIN; // maximum file descriptor number. initialised to listener    
    // variable initialisations
    int new_client_fd; // newly accepted socket descriptor
    struct sockaddr_storage new_client_addr; // client address
    socklen_t addrlen; // address length
    char data_buffer[500 * 200]; // buffer for client data
    int data_buffer_bytes; // holds number of bytes received and stored in data_buffer
    char newClientIP[INET6_ADDRSTRLEN]; // holds the ip of the new client
    int fd;

    // main loop
    while (1) {
        read_fds = master; // make a copy of master set
        if (select(fdmax + 1, & read_fds, NULL, NULL, NULL) == -1) {
            exit(EXIT_FAILURE);
        }

        // run through the existing connections looking for data to read
        for (fd = 0; fd <= fdmax; fd++) {
            if (FD_ISSET(fd, & read_fds)) {
                // if fd == listener, a new connection has come in.
                if (fd == sock) {
                    addrlen = sizeof new_client_addr;
                    new_client_fd = accept(sock, (struct sockaddr * ) & new_client_addr, & addrlen);

                    if (new_client_fd != -1) {
                        // We register the new client onto our system here.
                        // We store the new client details here. We will assign the values later when the 
                        // client sends more information about itself like the hostname
                        new_client = malloc(sizeof(struct host));
                        FD_SET(new_client_fd, & master); // add to master set
                        if (new_client_fd > fdmax) { // keep track of the max
                            fdmax = new_client_fd;
                        }
                        struct sockaddr * sa = (struct sockaddr * ) & new_client_addr;
                        if (sa -> sa_family == AF_INET) {
                            memcpy(new_client -> ip_addr,
                            inet_ntop(
                                new_client_addr.ss_family,
                                &(((struct sockaddr_in * ) sa) -> sin_addr), // even though new_client_addr is of type sockaddr_storage, they can be cast into each other. Refer beej docs.
                                newClientIP,
                                INET6_ADDRSTRLEN
                            ), sizeof(new_client -> ip_addr));
                        }
                        else{
                            memcpy(new_client -> ip_addr,
                            inet_ntop(
                                new_client_addr.ss_family,
                                &(((struct sockaddr_in6 * ) sa) -> sin6_addr), // even though new_client_addr is of type sockaddr_storage, they can be cast into each other. Refer beej docs.
                                newClientIP,
                                INET6_ADDRSTRLEN
                            ), sizeof(new_client -> ip_addr));
                        }
                        new_client -> fd = new_client_fd;
                        new_client -> num_msg_sent = 0;
                        new_client -> num_msg_rcv = 0;
                        new_client -> is_logged_in = 1;
                        new_client -> next_host = NULL;
                        new_client -> blocked = NULL;
                    }
                    fflush(stdout);
                } else if (fd == STDIN) {
                    // handle data from standard input
                    char * command = (char * ) malloc(sizeof(char) * 500 * 200);
                    memset(command, '\0', 500 * 200);
                    if (fgets(command, 500 * 200 - 1, stdin) == NULL) { // -1 because of new line
                    } else {
                        execute_command(myhost, command, fd, clients);
                    }
                    fflush(stdout);
                } else {
                    // handle data from a client
                    data_buffer_bytes = recv(fd, data_buffer, sizeof data_buffer, 0);
                    if (data_buffer_bytes == 0) {
                        close(fd); // Close the connection
                        FD_CLR(fd, & master); // Remove the fd from master set
                    } else if (data_buffer_bytes == -1) {
                        close(fd); // Close the connection
                        FD_CLR(fd, & master); // Remove the fd from master set
                    } else {
                        execute_command(myhost, data_buffer, fd, clients);
                    }
                    fflush(stdout);
                }
            }
        }
    }
    return;
}
