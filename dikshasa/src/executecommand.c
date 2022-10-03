#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/host.h"
#include "../include/universalMethods.h"
#include "../include/executecommand.h"
struct host host;


void host__print_list_of_clients(struct host* clients) {
    cse4589_print_and_log("[LIST:SUCCESS]\n");

    struct host * temp = clients;
    int id = 1;
    while (temp != NULL) {
        // SUSPICIOUS FOR REFRESH
        if (temp -> is_logged_in) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8s\n", id, temp -> hostname, temp -> ip_addr, (temp -> port_num));
            id = id + 1;
        }
        temp = temp -> next_host;
    }

    cse4589_print_and_log("[LIST:END]\n");
}

void execute_command(struct host *localhost, char command[], int requesting_client_fd, struct host* clients) {
    host__execute_command(localhost, command, requesting_client_fd);
    if (localhost -> hostType == 0) {
        server__execute_command(command, requesting_client_fd, clients);
    } else {
        client__execute_command(command, localhost, clients);
    }
    fflush(stdout);
}

/***  EXECUTE HOST COMMANDS (COMMAND SHELL COMMANDS) ***/
void host__execute_command(struct host *localhost, char command[], int requesting_client_fd) {
    if (strstr(command, "AUTHOR") != NULL) {
       printAuthor("dikshasa");
    } else if (strstr(command, "IP") != NULL) {
        displayIp(localhost-> ip_addr);
    } else if (strstr(command, "PORT") != NULL) {
        host__print_port(localhost -> port_num);
    }
    fflush(stdout);
}

/***  EXECUTE SERVER COMMANDS ***/
void server__execute_command(char command[], int requesting_client_fd, struct host* clients) {
    if (strstr(command, "LIST") != NULL) {
        host__print_list_of_clients(clients);
    } else if (strstr(command, "LOGIN") != NULL) {
        char client_hostname[250], client_port[250], client_ip[250];
        sscanf(command, "LOGIN %s %s %s", client_ip, client_port, client_hostname);
        server__handle_login(client_ip, client_port, client_hostname, requesting_client_fd);
    } else if (strstr(command, "REFRESH") != NULL) {
        server__handle_refresh(requesting_client_fd);
    } else if (strstr(command, "EXIT") != NULL) {
        server__handle_exit(requesting_client_fd);
    }
    fflush(stdout);
}

/***  EXECUTE CLIENT COMMANDS ***/
void client__execute_command(char command[], struct host *localhost, struct host* clients) {
    if (strstr(command, "LIST") != NULL) {
        if (localhost -> is_logged_in) {
            host__print_list_of_clients(clients);
        } else {
            cse4589_print_and_log("[LIST:ERROR]\n");
            cse4589_print_and_log("[LIST:END]\n");
        }
    } else if (strstr(command, "SUCCESSLOGIN") != NULL) {
        cse4589_print_and_log("[LOGIN:SUCCESS]\n");
        cse4589_print_and_log("[LOGIN:END]\n");
    } else if (strstr(command, "ERRORLOGIN") != NULL) {
        cse4589_print_and_log("[LOGIN:ERROR]\n");
        cse4589_print_and_log("[LOGIN:END]\n");
    } else if (strstr(command, "LOGIN") != NULL) { // takes two arguments server ip and server port
        char server_ip[250], server_port[250];
        int cmdi = 6;
        int ipi = 0;
        while (command[cmdi] != ' ' && ipi < 256) {
            server_ip[ipi] = command[cmdi];
            cmdi += 1;
            ipi += 1;
        }
        server_ip[ipi] = '\0';

        cmdi += 1;
        int pi = 0;
        while (command[cmdi] != '\0') {
            server_port[pi] = command[cmdi];
            cmdi += 1;
            pi += 1;
        }
        server_port[pi - 1] = '\0'; // REMOVE THE NEW LINE
        client__login(server_ip, server_port);
    }else if (strstr(command, "EXIT") != NULL) {
        client_exit();
    }
    fflush(stdout);
}