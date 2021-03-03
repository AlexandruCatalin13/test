#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include "list.h"
#include "thread_work.h"

void wait_routine(void)
{
	pthread_barrier_wait(&barrier);
	printf("Thread %lu started exection\n", pthread_self());
}

void *first_thread_work(void *p)
{

	wait_routine();

	struct list *head = (struct list *) p;
	add_node(&head, 2);
	add_node(&head, 4);
	add_node(&head, 10);
	delete_node(&head, 2);
	sort_list(&head);
	delete_node(&head, 10);
	delete_node(&head, 5);

	printf("Thread %lu finished execution\n", pthread_self());
	return (void *) head;

}

void *second_thread_work(void *p)
{
	wait_routine();

	struct list *head = (struct list *) p;
	add_node(&head, 11);
	add_node(&head, 1);
	delete_node(&head, 11);
	add_node(&head, 8);
	print_list(head);

	printf("Thread %lu finished execution\n", pthread_self());
	return (void *) head;

}

void *third_thread_work(void *p)
{
	wait_routine();

	struct list *head = (struct list *) p;
	add_node(&head, 30);
	add_node(&head, 25);
	add_node(&head, 100);
	sort_list(&head); 
	print_list(head);
	delete_node(&head, 100);

	printf("Thread %lu finished execution\n", pthread_self());
	return (void *) head;
}



