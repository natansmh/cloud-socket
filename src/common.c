#include "../include/common.h"

int resolve_address(const char *host, const char *port, struct addrinfo **res) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;      // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;  // TCP

    return getaddrinfo(host, port, &hints, res);
}

int send_all(int sock, const void *buf, size_t len) {
    const char *p = buf;
    while (len > 0) {
        ssize_t sent = send(sock, p, len, 0);
        if (sent <= 0) return -1;
        p   += sent;
        len -= sent;
    }
    return 0;
}

int recv_line(int sock, char *buf, size_t maxlen) {
    size_t pos = 0;
    while (pos < maxlen - 1) {
        char c;
        ssize_t r = recv(sock, &c, 1, 0);
        if (r <= 0) return -1;
        if (c == '\n') break;
        buf[pos++] = c;
    }
    buf[pos] = '\0';
    return 0;
}
