#include <string.h>

#include "s_custom.h"

socket_t socket_server_create(const char *ip, int port, int backlog)
{
	socket_t fd = socket_create(AF_INET, SOCK_STREAM, 0);
	if (fd == SOCKET_INVALID) return SOCKET_INVALID;

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = (strcmp(ip, "0.0.0.0") == 0) ? INADDR_ANY : inet_addr(ip);

	if (socket_bind(fd, (struct sockaddr*)&sa, sizeof(sa)) < 0) {
		socket_close(fd);
		return SOCKET_INVALID;
	}
	if (socket_listen(fd, backlog) < 0) {
		socket_close(fd);
		return SOCKET_INVALID;
	}
	return fd;
}

socket_t socket_client_create(const char *ip, int port)
{
	socket_t fd = socket_create(AF_INET, SOCK_STREAM, 0);
	if (fd == SOCKET_INVALID) {
		return SOCKET_INVALID;
	}

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = (strcmp(ip, "0.0.0.0") == 0) ? INADDR_ANY : inet_addr(ip);
	return fd;
}
