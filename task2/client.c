#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<mqueue.h>
#include<sys/types.h>
#include<unistd.h>

#include "queue.h"

int main(int argc, char **argv)
{	
	mqd_t server_descr, client_descr;
	unsigned int token;
	char recv_buffer[MAX_BUFFER_SIZE], client_name[20];
	char *data;
	struct mq_attr attr;
	int msg_len;

	attr.mq_flags = 0;
	attr.mq_maxmsg = MAX_MSG;
	attr.mq_msgsize = MAX_MSG_SIZE;
	attr.mq_curmsgs = 0;

	server_descr = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
	if (server_descr == -1) {
		perror("mq_open() failed on client - server creation\n");
		exit(server_descr);
	}
	
	/* setting the client queue name */
	msg_len = sprintf(client_name, "%s-%d", CLIENT_QUEUE_NAME, getpid());

	client_descr = mq_open(client_name, O_RDONLY | O_CREAT, PERMISSIONS, &attr);
	if (client_descr == -1) {
		perror("mq_open() failed on client - client creation\n");
		exit(client_descr);
	}

	while(1) {
		printf("Press enter to send a token request\nPress e to exit\n");
		if ('e' == getchar())
			exit(0);

		msg_len = mq_send(server_descr, client_name, strlen(client_name) + 1, 0);
		if (msg_len == -1) {
			perror("mq_send() failed from client\n");
			exit(msg_len);
		}

		msg_len = mq_receive(client_descr, recv_buffer, MAX_BUFFER_SIZE, NULL);
		if (msg_len == -1) {
			perror("mq_receive() failed from client\n");
			exit(msg_len);
		}
		
		data = malloc(5 * sizeof(char));
		strcpy(data, recv_buffer);
		token = atoi(data);
		printf("Received number %d from server\n", token);
	}

	if (mq_close(client_descr) == -1) {
		perror("mq_close() failed - client");
		exit(-1);
	}
	if (mq_close(server_descr) == -1) {
		perror("mq_close() failed - server");
		exit(-1);
	}

	if (mq_unlink(client_name) == -1) {
		perror("mq_unlink() failed");
		exit(-1);
	}
	
	return 0;
}
