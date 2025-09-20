#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

#include "../socket/socket.h"

socket_t socket_server_create(const char *ip, int port, int backlog);
socket_t socket_client_create(const char *ip, int port);

#endif
