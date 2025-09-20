#include <string.h>
#include <pthread.h>
#include <stdio.h>

#include "core/socket/socket.h"
#include "core/types.h"


#define PORT 5000
#define BUFFER_SIZE 512
#define MAX_CLIENTS 100

socket_t clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void broadcast(Packet p) {
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < client_count; i++) {
		if (socket_send(clients[i], &p, sizeof(Packet), 0) <= 0) {
			printf("Failed to send to client %d\n", i);
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
	socket_t sock = (socket_t)(intptr_t)arg;
	Packet recv_p;

	while (1) {
		ssize_t nb = socket_recv(sock, &recv_p, sizeof(Packet), 0);
		if (nb <= 0) {
			printf("Client disconnected\n");
			remove_client(sock);
			break;
		}

		switch (recv_p.type) {
		case T_MSG_TEXT:
			broadcast(recv_p);
			break;
		case T_PING:
			break;
		case T_AUTH:
			break;
		case T_DISCONNECT:
			printf("Client requested disconnect\n");
			remove_client(sock);
			socket_close(sock);
			return NULL;
		default:
			printf("Unknown packet type: %d\n", recv_p.type);
		}
	}

	socket_close(sock);
	return NULL;
}

int main() {
	int server_fd;
	struct sockaddr_in address;
	socklen_t addrlen = sizeof(address);

	socket_init();

	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket failed");
		return -1;
	}

	memset(&address, 0, sizeof(address));
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);

	if (socket_bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
		perror("bind failed");
		socket_close(server_fd);
		return -1;
	}

	if (socket_listen(server_fd, 3) < 0) {
		perror("listen failed");
		socket_close(server_fd);
		return -1;
	}

	printf("server listening on port %d...\n", PORT);

	while (1) {
		socket_t connfd = accept(server_fd, (struct sockaddr *)&address, &addrlen);
		if (connfd <= 0) {
			perror("accept failed");
			continue;
		}

		pthread_mutex_lock(&clients_mutex);
		if (client_count >= MAX_CLIENTS) {
			printf("Too many clients, rejecting connection\n");
			socket_close(connfd);
			pthread_mutex_unlock(&clients_mutex);
			continue;
		}
		clients[client_count++] = connfd;
		pthread_mutex_unlock(&clients_mutex);

		printf("Client connected\n");

		pthread_t tid;
		if (pthread_create(&tid, NULL, client_handler, (void *)(intptr_t)connfd) != 0) {
			perror("pthread_create failed");
			remove_client(connfd);
			socket_close(connfd);
			continue;
		}
		pthread_detach(tid);
	}


	socket_close(server_fd);
	socket_cleanup();
	return 0;
}
