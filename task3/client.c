#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/stat.h>
#include<netdb.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<errno.h>

#include "common.h"

#define localhost 	"127.0.0.1"
#define PORT		5555

int main(int argc, char **argv)
{
	int clientfd, rc = 0, client_writefd;
	int i;
	struct sockaddr_in server_addr;
	char buffer[MAX_BUFFER_SIZE] = { 0 };
	char *file_to_write = NULL;

	if (argc < 2) {
		perror("Usage: ./client file [files]");
		return 0;
	}

	rc = socket(AF_INET, SOCK_STREAM, 0);
	if (rc < 0) {
		perror("socket() failed");
		goto exit;
	}

	clientfd = rc;

	memset(&server_addr, '0', sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	inet_aton(localhost, &server_addr.sin_addr);

	rc = connect(clientfd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (rc == -1) {
		printf("%d\n", errno);
		perror("connect() failed");
		goto close_client;
	}

	file_to_write = malloc(MAX_BUFFER_SIZE * sizeof(char));

	for (i = 1; i < argc; i++) {

		memset(buffer, 0, MAX_BUFFER_SIZE);
		snprintf(buffer, MAX_BUFFER_SIZE, "%s", argv[i]);

		printf("Writing %s\n", buffer);

		rc = send(clientfd, buffer, strlen(argv[i]) + 1, 0);
		if (rc == -1) {
			perror("send() failed - file_name");
			goto close_client;
		}
		
		memset(file_to_write, 0, MAX_BUFFER_SIZE);
		snprintf(file_to_write, MAX_BUFFER_SIZE, "%s%s", CLIENT_FILES, argv[i]);

		printf("Data to be written at: %s\n", file_to_write);

		if (strcmp(buffer, "END") == 0)
			break;

		rc = open(file_to_write, O_CREAT | O_WRONLY | O_TRUNC, 0666);
		if (rc == -1) {
			perror("open() failed");
			goto close_client;
		}

		printf("Opened %s\n. Starting to write.\n", file_to_write);

		client_writefd = rc;
		rc = recv(clientfd, buffer, MAX_BUFFER_SIZE, 0);
		if (strcmp(buffer, "File not found") == 0) {
			printf("File is not on the server\n");
			remove(file_to_write);
			close(client_writefd);
			continue;
		}
		else {
			rc = write(client_writefd, buffer, rc);
			if (rc < MAX_BUFFER_SIZE)
				continue;
		}

		while(1) {
			rc = recv(clientfd, buffer, MAX_BUFFER_SIZE, 0);
			if (rc == -1) {
				perror("recv() failed");
				goto close_file;
			}

			if (strcmp(buffer, "END") == 0)
				break;
			
			rc = write(client_writefd, buffer, rc);

			if (rc < MAX_BUFFER_SIZE)
				break;
		}
		
		printf("Finished %s. ", file_to_write);
		close(client_writefd);
	}

	sprintf(buffer, "%s", "END");
	send(clientfd, buffer, 4, 0);

	free(file_to_write);

close_file:
	close(client_writefd);
close_client:
	close(clientfd);
exit:
	return rc;
}
