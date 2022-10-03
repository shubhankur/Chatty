struct host {
    char hostname[200];
    char ip_addr[200];
    char port_num[200];
    int num_msg_sent;
    int num_msg_rcv;
    char status[200];
    int fd;
    struct host * blocked;
    struct host * next_host;
    int is_logged_in;
    int hostType; //0 for server, 1 for client
    struct message * queued_messages;
};