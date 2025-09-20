#ifndef CCHAT_SOCKET_H
#define CCHAT_SOCKET_H

#ifdef _WIN32
	#include <winsock2.h>
	#include <ws2tcpip.h>
	typedef SOCKET socket_t;
	typedef int socklen_t;
	#define SOCKET_INVALID ((socket_t)INVALID_SOCKET)
	#ifndef _SSIZE_T_DEFINED
		typedef long ssize_t;
		#define _SSIZE_T_DEFINED
	#endif

#else
	/* POSIX */
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <unistd.h>
	#include <errno.h>
	typedef int socket_t;
	#define SOCKET_INVALID ((socket_t)(-1))
#endif

int socket_init(void);
void socket_cleanup(void);
socket_t socket_create(int domain, int type, int protocol);
int socket_bind(socket_t sock, const struct sockaddr *addr, socklen_t addrlen);
int socket_listen(socket_t sock, int backlog);
socket_t socket_accept(socket_t sock, struct sockaddr *addr, socklen_t *addrlen);
int socket_connect(socket_t sock, const struct sockaddr *addr, socklen_t addrlen);
ssize_t socket_send(socket_t sock, const void *buf, size_t len, int flags);
ssize_t socket_recv(socket_t sock, void *buf, size_t len, int flags);
void socket_close(socket_t sock);
int socket_get_last_error(void);

#endif
