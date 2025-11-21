#include "../include/common.h"
#include "../include/server.h"

int run_server(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return 1;
    }

    const char *port = argv[1];

    struct addrinfo hints, *res, *rp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_INET6;     // start with IPv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    int sock = -1;

    for (rp = res; rp != NULL; rp = rp->ai_next) {

        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0)
            continue;

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        // allow dual-stack
        if (rp->ai_family == AF_INET6) {
            int off = 0;
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
        }

        if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0)
            break;

        close(sock);
        sock = -1;
    }

    freeaddrinfo(res);

    if (sock < 0) {
        fprintf(stderr, "error: bind failed for all families\n");
        return 1;
    }

    if (listen(sock, 10) < 0) {
        perror("listen");
        close(sock);
        return 1;
    }

    printf("Server listening on port %s...\n", port);

    struct sockaddr_storage client_addr;
    socklen_t client_len = sizeof(client_addr);

    int client = accept(sock, (struct sockaddr *)&client_addr, &client_len);
    if (client < 0) {
        perror("accept");
        close(sock);
        return 1;
    }

    char buf[128];

    if (recv_line(client, buf, sizeof(buf)) < 0) {
        fprintf(stderr, "error reading READY\n");
        close(client);
        close(sock);
        return 1;
    }

    printf("Server received: %s\n", buf);

    send_all(client, READY_ACK_MSG, strlen(READY_ACK_MSG));
    send_all(client, "\n", 1);

    if (recv_line(client, buf, sizeof(buf)) == 0)
        printf("Server received: %s\n", buf);

    close(client);
    close(sock);
    return 0;
}
