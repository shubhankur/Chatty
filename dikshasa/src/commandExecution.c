#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include "../src/startup.c"

void execute_command(struct host *localhost, char command[], int requesting_client_fd) {
    host__execute_command(command, requesting_client_fd);
    if (localhost -> hostType == 0) {
        server__execute_command(command, requesting_client_fd);
    } else {
        client__execute_command(command, localhost);
    }
    fflush(stdout);
}

/***  EXECUTE HOST COMMANDS (COMMAND SHELL COMMANDS) ***/
void host__execute_command(char command[], int requesting_client_fd) {
    if (strstr(command, "AUTHOR") != NULL) {
        host__print_author();
    } else if (strstr(command, "IP") != NULL) {
        host__print_ip_address();
    } else if (strstr(command, "PORT") != NULL) {
        host__print_port();
    }
    fflush(stdout);
}

/***  EXECUTE SERVER COMMANDS ***/
void server__execute_command(char command[], int requesting_client_fd) {
    if (strstr(command, "LIST") != NULL) {
        host__print_list_of_clients();
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
void client__execute_command(char command[], struct host *localhost) {
    if (strstr(command, "LIST") != NULL) {
        if (localhost -> is_logged_in) {
            host__print_list_of_clients();
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
    }else if (strstr(command, "REFRESH") != NULL) {
        if (localhost -> is_logged_in) {
            host__send_command(server -> fd, "REFRESH\n");
        } else {
            cse4589_print_and_log("[REFRESH:ERROR]\n");
            cse4589_print_and_log("[REFRESH:END]\n");
        }
    } else if (strstr(command, "EXIT") != NULL) {
        client_exit();
    }
    fflush(stdout);
}