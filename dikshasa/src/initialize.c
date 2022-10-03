/***  referenced from https://gist.github.com/listnukira/4045436 ***/

#include "../include/host.h"
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

struct host * myhost = NULL;

void setHostNameAndIp(struct host * h) {
    char myIP[16];
    unsigned int myPort;
    struct sockaddr_in server_addr, my_addr;
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct hostent *he;

    // Set server_addr of Google DNS
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("8.8.8.8");
    server_addr.sin_port = htons(53);

    // Connect to server
    connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

    // Get my ip address and port
    bzero(&my_addr, sizeof(my_addr));
    socklen_t len = sizeof(my_addr);
    getsockname(sockfd, (struct sockaddr *)&my_addr, &len);
    inet_ntop(AF_INET, &my_addr.sin_addr, myIP, sizeof(myIP));
    he = gethostbyaddr(&my_addr.sin_addr, sizeof(my_addr.sin_addr), AF_INET);

    // Storing my IP and Address in myHost
    memcpy(h->ip, myIP, sizeof(h->ip));
    memcpy(h->hostname, he->h_name, sizeof(h->hostname));
    return;
}

/***  HOST INITIALISATION ***/
void inialize(bool is_server, char * port) {
    myhost = malloc(sizeof(struct host));
    memcpy(myhost -> port, port, sizeof(myhost -> port));
    myhost -> is_server = is_server;
    setHostNameAndIp(myhost);
    if (is_server) {
        initializeServer(myhost);
    } else {
        initializeClient(myhost);
    }
}