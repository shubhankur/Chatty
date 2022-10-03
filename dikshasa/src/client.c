#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/startup.c"
#include "../include/executecommand.h"


#define STDIN 0

int client__register_listener(struct host *myhost) {
    int listener = 0, status;
    struct addrinfo hints, * localhost_ai, * temp_ai;

    // get a socket and bind it
    memset( & hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if (status = getaddrinfo(NULL, myhost -> port_num, & hints, & localhost_ai) != 0) {
        exit(EXIT_FAILURE);
    }

    for (temp_ai = localhost_ai; temp_ai != NULL; temp_ai = temp_ai -> ai_next) {
        listener = socket(temp_ai -> ai_family, temp_ai -> ai_socktype, temp_ai -> ai_protocol);
        if (listener < 0) {
            continue;
        }
        setsockopt(listener, SOL_SOCKET, SO_REUSEPORT, & yes, sizeof(int));
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, & yes, sizeof(int));
        if (bind(listener, temp_ai -> ai_addr, temp_ai -> ai_addrlen) < 0) {
            close(listener);
            continue;
        }
        break;
    }

    // exit if could not bind
    if (temp_ai == NULL) {
        exit(EXIT_FAILURE);
    }

    // listen
    if (listen(listener, 10) == -1) {
        exit(EXIT_FAILURE);
    }

    myhost -> fd = listener;

    freeaddrinfo(localhost_ai);
}

void client__init(struct host *myhost) {
    // TODO: modularise
    client__register_listener(myhost);
    while (1) {
        // handle data from standard input
        char * command = (char * ) malloc(sizeof(char) * 500*500);
        memset(command, '\0', 500*500);
        if (fgets(command, 500*500, stdin) != NULL) {
            execute_command(myhost, command, STDIN);
        }
    }
}