#include <stdio.h>
#include "../include/universalMethods.h"
#include "../include/global.h"
#include "../include/logger.h"

void printAuthor(char *ubit)
{
    cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
    cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", ubit);
    cse4589_print_and_log("[AUTHOR:END]\n");
}

/***  PRINT IP ***/
void displayIp(char *ip)
{
    cse4589_print_and_log("[IP:SUCCESS]\n");
    cse4589_print_and_log("IP:%s\n", ip);
    cse4589_print_and_log("[IP:END]\n");
}

/***  PRINT PORT ***/
void displayPort(char *port)
{
    cse4589_print_and_log("[PORT:SUCCESS]\n");
    cse4589_print_and_log("PORT:%s\n", port);
    cse4589_print_and_log("[PORT:END]\n");
}