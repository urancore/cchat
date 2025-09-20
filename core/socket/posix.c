#ifndef _WIN32

#include <stdio.h>

#include "socket.h"

int socket_init(void) {
	(void)0;
	return 0;
}

void socket_cleanup(void) {
	(void)0;
}

socket_t socket_create(int domain, int type, int protocol) {
	int s = socket(domain, type, protocol);
	if (s < 0) return SOCKET_INVALID;
	return (socket_t)s;
}

int socket_bind(socket_t sock, const struct sockaddr *addr, socklen_t addrlen) {
	int r = bind(sock, addr, addrlen);
	return (r == -1) ? -1 : 0;
}

int socket_listen(socket_t sock, int backlog) {
	int r = listen(sock, backlog);
	return (r == -1) ? -1 : 0;
}

socket_t socket_accept(socket_t sock, struct sockaddr *addr, socklen_t *addrlen) {
	int s = accept(sock, addr, addrlen);
	if (s < 0) return SOCKET_INVALID;
	return (socket_t)s;
}

int socket_connect(socket_t sock, const struct sockaddr *addr, socklen_t addrlen) {
	int r = connect(sock, addr, addrlen);
	return (r == -1) ? -1 : 0;
}

ssize_t socket_send(socket_t sock, const void *buf, size_t len, int flags) {
	ssize_t r = send(sock, buf, len, flags);
	return (r == -1) ? -1 : r;
}

ssize_t socket_recv(socket_t sock, void *buf, size_t len, int flags) {
	ssize_t r = recv(sock, buf, len, flags);
	return (r == -1) ? -1 : r;
}

void socket_close(socket_t sock) {
	close(sock);
}

int socket_get_last_error(void) {
	return errno;
}
#endif
