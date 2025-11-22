#include <netdb.h>
#include <stdbool.h>
#include "../include/common.h"
#include "../include/client.h"

static int handle_exchange(int sock) {
    send_all(sock, READY_MSG, strlen(READY_MSG));
    send_all(sock, "\n", 1);

    char buf[128];
    if (recv_line(sock, buf, sizeof(buf)) < 0) {
        fprintf(stderr, "error reading ACK\n");
        return -1;
    }

    printf("Client received: %s\n", buf);

    send_all(sock, BYE_MSG, strlen(BYE_MSG));
    send_all(sock, "\n", 1);

    return 0;
}

int run_client(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s <host> <port> <directory>\n", argv[0]);
        return 1;
    }

    const char *host = argv[1];
    const char *port = argv[2];

    struct addrinfo *res;
    if (resolve_address(host, port, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock < 0) {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    if (connect(sock, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        close(sock);
        freeaddrinfo(res);
        return 1;
    }

    if (handle_exchange(sock) < 0)
        fprintf(stderr, "error during exchange\n");

    close(sock);
    freeaddrinfo(res);
    return 0;
}
