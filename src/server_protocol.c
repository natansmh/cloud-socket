#include "../include/protocols.h"
#include "../include/common.h"

static int expect_ready(int client_sock, char *buf, size_t bufsize) {
    if (recv_line(client_sock, buf, bufsize) < 0)
        return -1;

    if (strcmp(buf, READY_MSG) != 0)
        return -1;

    printf("Server received: %s\n", buf);
    return 0;
}

static int expect_send_dir(int client_sock, char *buf, size_t bufsize) {
    if (recv_line(client_sock, buf, bufsize) < 0)
        return -1;

    if (strncmp(buf, "SEND DIR ", 9) != 0)
        return -1;

    printf("Server received: %s\n", buf);
    return 0;
}

int server_protocol(int client_sock) {
    char buf[512];

    if (expect_ready(client_sock, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "error: READY not received\n");
        return -1;
    }

    if (send_line(client_sock, READY_ACK_MSG) < 0)
        return -1;

    if (expect_send_dir(client_sock, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "error: SEND DIR not received\n");
        return -1;
    }

    const char *path = buf + 9;

    if (send_line(client_sock, OK_MSG) < 0)
        return -1;

    // Loop until done
    for (;;) {
        if (recv_line(client_sock, buf, sizeof(buf)) < 0)
            return -1;

        if (strcmp(buf, DONE_MSG) == 0)
            break;

        if (strncmp(buf, "FILE ", 5) == 0) {
            // To implement:
            // - extract filename and size
            // - receive raw content
            // - wait

            // For now, just discard
            // read_filename_and_content(client_sock, buf);
            continue;
        }

        return -1;
    }

    if (send_line(client_sock, BYE_MSG) < 0)
        return -1;

    return 0;
}
