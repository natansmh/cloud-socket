#ifndef PROTOCOLS_H
#define PROTOCOLS_H

int server_protocol(int client_sock);
int client_protocol(int sock, const char *directory);

#endif
