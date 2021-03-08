#include<stdio.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>
#include<errno.h>

#include "queue.h"

int main(int argc, char **argv)
{

	mqd_t server_descr, client_descr;
	unsigned int token = 0;
	char recv_buffer[MAX_BUFFER_SIZE], send_buffer[MAX_BUFFER_SIZE];
	struct mq_attr attr;
	int msg_len;

	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	server_descr = mq_open(SERVER_QUEUE_NAME, O_RDONLY | O_CREAT, PERMISSIONS, &attr);
	if (server_descr == -1) {
		perror("mq_open() from server failed - server creation\n");
		exit(server_descr);
	}

	while(1) {
		printf("Waiting for a new request. Press e to exit\n");
		if ('e' == getchar())
			break;

		msg_len = mq_receive(server_descr, recv_buffer, MAX_BUFFER_SIZE, NULL);
		if (msg_len == -1) {
			perror("mq_receive() from server failed");
			exit(msg_len);
		}

		client_descr = mq_open(recv_buffer, O_WRONLY);
		if (client_descr == -1) {
			perror("mq_open() from server failed - client creation");
			exit(client_descr);
		}
		msg_len = sprintf(send_buffer, "%d", token);
		msg_len = mq_send(client_descr, send_buffer, msg_len + 1, 0);
		if (msg_len == -1) {
			perror("mq_send() from server failed\n");
			exit(msg_len);
		}
		token++;
	}

	return 0;
}
