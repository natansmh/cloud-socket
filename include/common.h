#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>

#define READY_MSG "READY"
#define READY_ACK_MSG "READY ACK"
#define BYE_MSG "bye"

#define DEFAULT_BUF 1024

int resolve_address(const char *host, const char *port, struct addrinfo **res);
int send_all(int sock, const void *buf, size_t len);
int recv_line(int sock, char *buf, size_t maxlen);

#endif
