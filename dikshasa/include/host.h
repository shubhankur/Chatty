struct host {
    char hostname[500];
    char ip[500];
    char port[500];
    char status[500];
    int fd;
    struct host * next_host;
    bool is_logged_in;
    bool is_server;
};