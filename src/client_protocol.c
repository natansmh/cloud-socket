#include "../include/protocols.h"
#include "../include/common.h"

static int expect_ready_ack(int sock, char *buf, size_t bufsize) {
    if (recv_line(sock, buf, bufsize) < 0)
        return -1;

    if (strcmp(buf, READY_ACK_MSG) != 0)
        return -1;

    printf("Server received: %s\n", buf);
    return 0;
}

static int expect_ok(int sock, char *buf, size_t bufsize) {
    if (recv_line(sock, buf, bufsize) < 0)
        return -1;

    if (strcmp(buf, OK_MSG) != 0)
        return -1;

    printf("Server received: %s\n", buf);
    return 0;
}

static int expect_bye(int sock, char *buf, size_t bufsize) {
    if (recv_line(sock, buf, bufsize) < 0)
        return -1;

    if (strcmp(buf, BYE_MSG) != 0)
        return -1;

    printf("Server received: %s\n", buf);
    return 0;
}

int client_protocol(int sock, const char *directory) {
    if (send_line(sock, READY_MSG) < 0)
        return -1;

    char buf[512];

    if (expect_ready_ack(sock, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "error: READY ACK not received\n");
        return -1;
    }

    char cmd[600];
    snprintf(cmd, sizeof(cmd), "SEND DIR %s", directory);
    if (send_line(sock, cmd) < 0)
        return -1;

    if (expect_ok(sock, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "error: OK not received\n");
        return -1;
    }

    // To implement:
    // - send files recursively
    // send_all_files(sock, directory);

    if (send_line(sock, DONE_MSG) < 0)
        return -1;

    if (expect_bye(sock, buf, sizeof(buf)) != 0) {
        fprintf(stderr, "error: BYE not received\n");
        return -1;
    }

    return 0;
}
