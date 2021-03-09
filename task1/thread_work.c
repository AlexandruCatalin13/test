#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include "list.h"
#include "thread_work.h"

void *first_thread_work(void *p)
{

	struct list **head = (struct list  **) p;

	if (*head == NULL)
		return NULL;

	pthread_barrier_wait(&barrier);
	add_node(head, 2);
	add_node(head, 4);
	add_node(head, 10);
	delete_node(head, 2);
	sort_list(head);
	delete_node(head, 10);
	delete_node(head, 5);

	return (void *) head;

}

void *second_thread_work(void *p)
{

	struct list **head = (struct list **) p;

	if (*head == NULL)
		return NULL;

	pthread_barrier_wait(&barrier);
	add_node(head, 11);
	add_node(head, 1);
	delete_node(head, 11);
	add_node(head, 8);
	print_list(*head);

	return (void *) head;

}

void *third_thread_work(void *p)
{
	
	struct list **head = (struct list **) p;

	if (*head == NULL)
		return NULL;

	pthread_barrier_wait(&barrier);
	add_node(head, 30);
	add_node(head, 25);
	add_node(head, 100);
	sort_list(head); 
	print_list(*head);
	delete_node(head, 100);

	return (void *) head;
}



