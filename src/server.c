#include <stdbool.h>
#include "../include/common.h"
#include "../include/server.h"

/* Helpers Region */
static bool valid_call(int argc) {
    return argc == 2;
}

static bool valid_socket(int sock) {
    return sock != -1;
}

static bool valid_client(int client) {
    return client != -1;
}

static void init_hints(struct addrinfo *hints) {
    memset(hints, 0, sizeof(*hints));
    hints->ai_family   = AF_INET6;
    hints->ai_socktype = SOCK_STREAM;
    hints->ai_flags    = AI_PASSIVE;
}

static int bind_any_family(struct addrinfo *res) {
    struct addrinfo *rp;
    int sock = -1;

    for (rp = res; rp != NULL; rp = rp->ai_next) {
        sock = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sock < 0)
            continue;

        int opt = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        if (rp->ai_family == AF_INET6) {
            int off = 0;
            setsockopt(sock, IPPROTO_IPV6, IPV6_V6ONLY, &off, sizeof(off));
        }

        if (bind(sock, rp->ai_addr, rp->ai_addrlen) == 0)
            return sock;

        close(sock);
        sock = -1;
    }

    return -1;
}

static int accept_client(int server_sock) {
    struct sockaddr_storage addr;
    socklen_t len = sizeof(addr);
    return accept(server_sock, (struct sockaddr *)&addr, &len);
}

static int handle_initial_exchange(int client) {
    char buf[128];

    if (recv_line(client, buf, sizeof(buf)) < 0)
        return -1;

    printf("Server received: %s\n", buf);

    send_all(client, READY_ACK_MSG, strlen(READY_ACK_MSG));
    send_all(client, "\n", 1);

    if (recv_line(client, buf, sizeof(buf)) == 0)
        printf("Server received: %s\n", buf);

    return 0;
}
/* Helpers Region End */

int run_server(int argc, char *argv[]) {
    if (!valid_call(argc)) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return 1;
    }

    const char *port = argv[1];

    struct addrinfo hints, *res;
    init_hints(&hints);

    if (getaddrinfo(NULL, port, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    int sock = bind_any_family(res);
    freeaddrinfo(res);

    if (!valid_socket(sock)) {
        fprintf(stderr, "error: bind failed for all families\n");
        return 1;
    }

    if (listen(sock, 10) < 0) {
        perror("listen");
        close(sock);
        return 1;
    }

    printf("Server listening on port %s...\n", port);

    int client = accept_client(sock);
    if (!valid_client(client)) {
        perror("accept");
        close(sock);
        return 1;
    }

    if (handle_initial_exchange(client) < 0)
        fprintf(stderr, "error during initial exchange\n");

    close(client);
    close(sock);
    return 0;
}
