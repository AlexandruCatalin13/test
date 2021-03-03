#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "list.h"
#include "thread_work.h"

struct list *saved_head = NULL;

void print_val(struct list* node)
{
	printf("%d ", node->val);
	return;
}

struct list *create_node(int val)
{
	struct list *new_node = malloc(sizeof(struct list));
	if (new_node == NULL) {
		perror("create_head() malloc-failed");
		return NULL;
	}

	new_node->val = val;
	new_node->print_val = print_val;
	new_node->next = NULL;
	return new_node;
}

void print_list(struct list* head)
{
	pthread_mutex_lock(&list_mutex);
	struct list *tmp = head;

	printf("Thread %lu is doing print_list\n", pthread_self());
	while(tmp != NULL) {
		tmp->print_val(tmp);
		tmp = tmp->next;
	}
	printf("\n");
	printf("Thread %lu finished print_list\n", pthread_self());
	pthread_mutex_unlock(&list_mutex);
}

struct list *add_node(struct list **head, int val)
{
	pthread_mutex_lock(&list_mutex);
	struct list* new_node = create_node(val);

	printf("Thread %lu is doing add_node, value: %d\n", pthread_self(), val);

	if (*head == NULL && saved_head == NULL) {
		*head = new_node;
		saved_head = *head;
		printf("Thread %lu finished add_node, value: %d\n", pthread_self(), val);
		pthread_mutex_unlock(&list_mutex);
		return *head;
	}

	if (*head == NULL && saved_head != NULL)
		*head = saved_head;
	
	struct list *tmp = *head;

	while(tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = new_node;
	printf("Thread %lu finished add_node, value :%d\n", pthread_self(), val);
	pthread_mutex_unlock(&list_mutex);

	return *head;

}

void swap(struct list *a, struct list *b)
{
	int tmp = a->val;
	a->val = b->val;
	b->val = tmp;
}

struct list *sort_list(struct list **head)
{

	int swapped;
	struct list *fast = NULL, *slow = NULL;

	if (*head == NULL)
		return *head;

	/* bubble sort */
	pthread_mutex_lock(&list_mutex);
	printf("Thread %lu is doing sort_list\n", pthread_self());

	do 
	{
		swapped = 0;
		fast = *head;

		while (fast->next != slow) {
			if (fast->val > fast->next->val) {
				swap(fast, fast->next);
				swapped = 1;
			}
			fast = fast->next;
		}
		slow = fast;
	} while (swapped);

	saved_head = *head;

	printf("Thread %lu finished sort_list\n", pthread_self());
	pthread_mutex_unlock(&list_mutex);

	return *head;
}

void flush_list(struct list *head)
{
	while(head) {
		struct list *tmp = head;
		head = head->next;
		free(tmp);
	}
}

struct list *delete_node(struct list **head, int val)
{

	/* if the first node is the one
	 * that has to be deleted */

	pthread_mutex_lock(&list_mutex);
	struct list *tmp = *head;
	struct list *prev = NULL;
	printf("Thread %lu is doing delete_node, value: %d\n", pthread_self(), val);

	if (tmp != NULL && tmp->val == val)
	{
		*head = tmp->next;
		saved_head = tmp->next;
		free(tmp);
		printf("Thread %lu finished delete_node, value: %d\n", pthread_self(), val);
		pthread_mutex_unlock(&list_mutex);
		return *head;
	}
	else {
		while(tmp != NULL && tmp->val != val) {
			prev = tmp;
			tmp = tmp->next;
		}

		if (tmp == NULL) {
			printf("The value to be deleted does not exist: %d\n", val);
			pthread_mutex_unlock(&list_mutex);
			return NULL;
		}

		prev->next = tmp->next;
		saved_head = *head;

		free(tmp);
	}

	printf("Thread %lu finished delete_node, value: %d\n", pthread_self(), val);
	pthread_mutex_unlock(&list_mutex);
	/* if you get here, the value does not exist. */
	return *head;
}
