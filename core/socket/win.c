#include <stdio.h>

#include "socket.h"


int socket_init(void)
{
	WSADATA wsa;
	int r = WSAStartup(MAKEWORD(2,2), &wsa);
	return r;
}

void socket_cleanup(void) { WSACleanup(); }

socket_t socket_create(int domain, int type, int protocol)
{
	SOCKET s = socket(domain, type, protocol);
	if (s == INVALID_SOCKET) return SOCKET_INVALID;
	return (socket_t)s;
}

int socket_bind(socket_t sock, const struct sockaddr *addr, socklen_t addrlen)
{
	int r = bind((SOCKET)sock, addr, addrlen);
	return (r == SOCKET_ERROR) ? -1 : 0;
}

int socket_listen(socket_t sock, int backlog)
{
	int r = listen((SOCKET)sock, backlog);
	return (r == SOCKET_ERROR) ? -1 : 0;
}

socket_t socket_accept(socket_t sock, struct sockaddr *addr, socklen_t *addrlen)
{
	SOCKET s = accept((SOCKET)sock, addr, addrlen);
	if (s == INVALID_SOCKET) return SOCKET_INVALID;
	return (socket_t)s;
}

int socket_connect(socket_t sock, const struct sockaddr *addr, socklen_t addrlen)
{
	int r = connect((SOCKET)sock, addr, addrlen);
	return (r == SOCKET_ERROR) ? -1 : 0;
}

ssize_t socket_send(socket_t sock, const void *buf, size_t len, int flags)
{
	int r = send((SOCKET)sock, (const char*)buf, (int)len, flags);
	if (r == SOCKET_ERROR) return -1;
	return (ssize_t)r;
}

ssize_t socket_recv(socket_t sock, void *buf, size_t len, int flags)
{
	int r = recv((SOCKET)sock, (char*)buf, (int)len, flags);
	if (r == SOCKET_ERROR) return -1;
	return (ssize_t)r;
}

void socket_close(socket_t sock) { closesocket((SOCKET)sock); }
int socket_get_last_error(void)  { return WSAGetLastError();  }
