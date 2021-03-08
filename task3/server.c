#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<stdlib.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>

#include "common.h"

void *thread_work(void *p) {

	int rc, client_file_fd;
	char buffer[MAX_BUFFER_SIZE] = { 0 };
	char *current_file = NULL;

	int clientfd = *(int *)p;
	while (1) {
		rc = read(clientfd, buffer, MAX_BUFFER_SIZE);
		printf("I read %s\n", buffer);

		/* if the client sends END, the thread ends too. */
		if (strcmp(buffer, "END") == 0) {
			if (current_file == NULL)
				goto exit;
			else
				goto free_current_file;
		}
			
		/* current_file has the format ./server_files/file
		 * where file is what is received in the buffer */
		if (current_file == NULL) {
			current_file = malloc(MAX_BUFFER_SIZE * sizeof(char));
			if (current_file == NULL) {
				perror("malloc() failed");
				goto exit;
			}
		}
		else 
			current_file = memset(current_file, 0, MAX_BUFFER_SIZE);

		sprintf(current_file, "%s%s", SERVER_FILES, buffer);
		printf("%s\n", current_file);

		rc = access(current_file, F_OK);
		if (rc == -1) {
			sprintf(buffer, "%s", "File not found");
			send(clientfd, buffer, strlen(buffer), 0);
			continue;
		}

		rc = open(current_file, O_RDONLY);
		if (rc == -1) {
			perror("open() failed");
			goto free_current_file;
		}

		client_file_fd = rc;
		printf("i opened the file %s with fd %d\n", current_file,  client_file_fd);

		while(1) {
			rc = read(client_file_fd, buffer, MAX_BUFFER_SIZE);

			if (rc == 0) {
				write(clientfd, "END\0", 4);
				break;
			}

			if (rc < 0) {
				perror("read() failed");
				goto close_file;
			}

			rc = write(clientfd, buffer, rc);
			if (rc <= 0) {
				perror("write() error");
				goto close_file;
			}

		}

		close(client_file_fd);
		printf("closed the file %s\n.getting the next file\n", current_file);
	}

close_file:
	close(client_file_fd);
free_current_file:
	free(current_file);
exit:	
	return (void *) 0;
}

int main(int argc, char **argv)
{
	int sockfd, clientfd, rc = 0;
	int current_thread = 0;
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	pthread_t tid[MAX_CLIENT];

	if (argc != 2) {
		printf("Usage: ./server port_number\n");
		return rc;
	}

	/* if provided port is higher then the limit (65535),
	 * we still try to make it work. */

	unsigned int server_port = atoi(argv[1]);
	if (server_port > 0xFFFF) 
		server_port = server_port % 0xFFFF;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == 0) {
		perror("socket() failed");
		rc = sockfd;
		goto exit;
	}

	printf("Using port %d\n", server_port);
	memset(&addr, '0', sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(server_port);

	inet_aton("127.0.0.1", &addr.sin_addr);

	rc = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	if (rc == -1) {
		perror("bind() failed");
		goto close_socket;
	}

	rc = listen(sockfd, MAX_CLIENT);
	if (rc == -1) {
		perror("listen() failed");
		goto close_socket;
	}
	

	while(1) {
		clientfd = accept(sockfd, (struct sockaddr *) &addr, (socklen_t *) &addr_len);
		if (clientfd == -1) {
			perror("accept() failed");
			goto close_socket;
		}

		printf("Accepted a new client %d\n", clientfd);

		/* it's a bit buffer overflow here. if you dont check the current_thread,
		 * at some point you will manage to write over the bounds of the tid vector. 
		 * this is just a temp solution */

		if (current_thread >= MAX_CLIENT) {
			printf("Can't create more threads\n. Exiting\n");
			rc = 0;
			break;
		}

		rc = pthread_create(&tid[current_thread], NULL, thread_work, &clientfd);
		if (rc != 0) {
			perror("pthread_create() failed");
			goto close_socket;
		}
		
		rc = pthread_join(tid[current_thread++], NULL);
		if (rc != 0) {
			perror("pthread_join() error");
			goto close_socket;
		}
	}

close_socket:
	close(sockfd);
exit:	
	return rc;
}
