#include <string.h>
#include <pthread.h>

#include "utils/logger/logger.h"
#include "core/socket/socket.h"
#include "core/types.h"

#define SERVER_IP "127.0.0.1"
#define PORT 5000
#define BUFFER_SIZE 512

typedef struct {
	socket_t sock;
	Logger *logger;
} thread_args_t;

void *send_thread(void *arg) {
	thread_args_t *args = (thread_args_t *)arg;
	char msg[BUFFER_SIZE];

	while (fgets(msg, sizeof(msg), stdin) != NULL) {
		Packet p;
		p.type = T_MSG_TEXT;

		strncpy(p.data.text_message.username, "urancore", sizeof(p.data.text_message.username)-1);
		p.data.text_message.username[sizeof(p.data.text_message.username)-1] = '\0';

		snprintf(p.data.text_message.text, sizeof(p.data.text_message.text), "%s", msg);
		p.data.text_message.size = strlen(p.data.text_message.text);
		p.data.text_message.user_id = args->sock;

		socket_send(args->sock, &p, sizeof(Packet), 0);
	}
	return NULL;
}

void *recv_thread(void *arg) {
	thread_args_t *args = (thread_args_t *)arg;

	while (1) {
		Packet p;
		int n = socket_recv(args->sock, &p, sizeof(p), 0);
		if (n > 0 && p.type == T_MSG_TEXT) {
			printf("%d:%s: %s", p.data.text_message.user_id, p.data.text_message.username, p.data.text_message.text);
		}

		if (n <= 0) {
			log_info(args->logger, "server disconnected");
			break;
		}
	}

	return NULL;
}


int main() {
	Logger l = logger_init(INFO, NULL, NULL, 1, 1);
	if (socket_init() != 0) {
		log_fatal(&l, "socket_init failed");
		return -1;
	}

	socket_t sock = socket_create(AF_INET, SOCK_STREAM, 0);
	if (sock == SOCKET_INVALID) {
		log_fatal(&l, "socket_create failed");
		return -1;
	}

	struct sockaddr_in server_addr;
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if (socket_connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		log_fatal(&l, "connect failed error=%d", socket_get_last_error());
		socket_close(sock);
		return -1;
	}

	log_info(&l, "connected to server %s:%d", SERVER_IP, PORT);

	thread_args_t args = { sock, &l };

	pthread_t tid_send, tid_recv;
	pthread_create(&tid_send, NULL, send_thread, &args);
	pthread_create(&tid_recv, NULL, recv_thread, &args);

	pthread_join(tid_send, NULL);

	socket_close(sock);

	pthread_join(tid_recv, NULL);

	socket_cleanup();
	return 0;
}
