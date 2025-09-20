#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "core/socket/socket.h"
#include "utils/logger/logger.h"
#include "core/types.h"
#include "utils/time/time.h"

typedef struct {
	socket_t sock;
	Logger *logger;
} thread_args_t;

#define PORT 5000
#define MAX_CLIENTS 100

socket_t clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(Packet p) {
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < client_count; i++) {
		if (socket_send(clients[i], &p, sizeof(Packet), 0) <= 0) {
			printf("failed to send to client %d\n", i);
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void remove_client(socket_t sock) {
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < client_count; i++) {
		if (clients[i] == sock) {
			clients[i] = clients[client_count - 1];
			client_count--;
			break;
		}
	}
	pthread_mutex_unlock(&clients_mutex);
}

void *client_handler(void *arg) {
	thread_args_t *args = (thread_args_t *)arg;
	Packet recv_p;

	while (1) {
		ssize_t nb = socket_recv(args->sock, &recv_p, sizeof(Packet), 0);
		if (nb <= 0) {
			log_info(args->logger, "client disconnected");
			remove_client(args->sock);
			break;
		}

		switch (recv_p.type) {
		case T_MSG_TEXT:
			broadcast(recv_p);
			break;
		case T_PING:
			Packet p;
			p.type = T_PING;
			p.data.ping.timestamp = get_timestamp()-recv_p.data.ping.timestamp;
			p.data.ping.user_id = recv_p.data.ping.user_id;

			socket_send(args->sock, &p, sizeof(Packet), 0);
			break;
		case T_AUTH:
			break;
		case T_DISCONNECT:
			log_info(args->logger, "client requested disconnect\n");
			remove_client(args->sock);
			socket_close(args->sock);
			return NULL;
		default:
			log_warn(args->logger, "unknown packet type: %d\n", recv_p.type);
		}
	}

	socket_close(args->sock);
	return NULL;
}

int main() {
	Logger l = logger_init(LOG_DEBUG, "server", NULL, 1, 1);
	int server_fd;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	socket_init();

	if ((server_fd = socket_create(AF_INET, SOCK_STREAM, 0)) < 0) {
		log_fatal(&l, "socket failed");
		return -1;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (socket_bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		log_fatal(&l, "bind failed");
		socket_close(server_fd);
		return -1;
	}

	if (socket_listen(server_fd, 10) < 0) {
		log_fatal(&l, "listen failed");
		socket_close(server_fd);
		return -1;
	}

	log_info(&l, "server listening on port %d...", PORT);

	while (1) {
		socket_t connfd = socket_accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (connfd <= 0) {
			log_fatal(&l, "accept failed");
			continue;
		}

		pthread_mutex_lock(&clients_mutex);
		if (client_count >= MAX_CLIENTS) {
			log_info(&l, "too many clients, rejecting connection");
			socket_close(connfd);
			pthread_mutex_unlock(&clients_mutex);
			continue;
		}

		clients[client_count++] = connfd;
		pthread_mutex_unlock(&clients_mutex);

		log_info(&l, "(%d) client connected", connfd);

		thread_args_t *args = malloc(sizeof(thread_args_t));
		args->sock = connfd;
		args->logger = &l;

		pthread_t tid;
		if (pthread_create(&tid, NULL, client_handler, args) != 0) {
			log_fatal(&l, "pthread_create failed");
			remove_client(connfd);
			socket_close(connfd);
			free(args);
			continue;
		}

		pthread_detach(tid);
	}


	socket_close(server_fd);
	socket_cleanup();
	return 0;
}
