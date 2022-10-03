void sendCommand(int fd, char msg[]) {
    int rv;
    if (rv = send(fd, msg, strlen(msg) + 1, 0) == -1) {
    }
}