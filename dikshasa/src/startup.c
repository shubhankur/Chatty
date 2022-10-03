#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include "../include/client.h"
#include "../include/host.h"

int yes = 1;

struct host *myHost = NULL;
struct host * new_client = NULL;
struct host * clients = NULL;



void host__init(char *hostType, char *port)
{
    myHost = malloc(sizeof(struct host));
    memcpy(myHost->port_num, port, sizeof(myHost->port_num)); // copy port number from input into localhost;
    host__set_hostname_and_ip(myHost);
    if (strcomp(hostType,'s'))
    {
        myHost->hostType = 0;
        server__init(myHost);
    }
    else
    {
        myHost->hostType = 1;
        client__init(myHost);
    }
}

void host__set_hostname_and_ip(struct host *h)
{
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
    memcpy(h->ip_addr, myIP, sizeof(h->ip_addr));
    memcpy(h->hostname, he->h_name, sizeof(h->hostname));
    return;
}