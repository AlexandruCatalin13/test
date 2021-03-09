#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<limits.h>

#include "list.h"
#include "thread_work.h"

static void cleanup(struct list *head)
{
	int rc;

	rc = pthread_mutex_destroy(&list_mutex);
	if (rc != 0) {
		perror("pthread_mutex_destroy() failed\n");
		exit(rc);
	}

	rc = pthread_barrier_destroy(&barrier);
	if (rc != 0) {
		perror("pthread_barrier_desotry() failed\n");
		exit(rc);
	}

	if (head == NULL)
		return;

	flush_list(&head);

	return;
}

int main(void)
{

	int i, rc;
	struct list *head = malloc(sizeof(struct list));
	head->val = INT_MIN;
	pthread_t id[3];

	rc = pthread_mutex_init(&list_mutex, NULL);
	if (rc != 0) {
		perror("pthread_mutex_init() failed\n");
		exit(rc);
	}
	pthread_barrier_init(&barrier, NULL, 3);
	if (rc != 0) {
		perror("pthread_barrier_init() failed\n");
		exit(rc);
	}

	rc = pthread_create(&id[0], NULL, first_thread_work, &head);
	if (rc != 0) {
		perror("pthread_create first thread failed\n");
		exit(rc);
	}

	rc = pthread_create(&id[1], NULL, second_thread_work, &head);
	if (rc != 0) {
		perror("pthread_create first thread failed\n");
		exit(rc);
	}

	rc = pthread_create(&id[2], NULL, third_thread_work, &head);
	if (rc != 0) {
		perror("pthread_create first thread failed\n");
		exit(rc);
	}

	printf("Main releasing threads\n");

	for(i = 0; i < 3; i++)
		pthread_join(id[i], NULL);

	printf("Printing list from main: \n");
	print_list(head);

	cleanup(head);

	return 0;
}

