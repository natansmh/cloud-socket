#include "../include/client.h"
#include "../include/common.h"
#include "../include/protocols.h"
#include <netdb.h>
#include <stdbool.h>

int run_client(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "usage: %s <host> <port> <directory>\n", argv[0]);
        return 1;
    }

    const char *host = argv[1];
    const char *port = argv[2];
    const char *directory = argv[3];

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

    if (client_protocol(sock, directory) < 0)
        fprintf(stderr, "error during exchange\n");

    close(sock);
    freeaddrinfo(res);
    return 0;
}
