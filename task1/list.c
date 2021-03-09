#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<limits.h>
#include<sys/syscall.h>
#include<unistd.h>

#include "list.h"
#include "thread_work.h"

void __print_val(struct list* node)
{
	if (node == NULL)
		return;
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
	new_node->print_val = &__print_val;
	new_node->next = NULL;
	return new_node;
}

void print_list(struct list* head)
{
	pthread_mutex_lock(&list_mutex);

	printf("Thread %ld is doing print_list\n", syscall(SYS_gettid));
	struct list *tmp = head;

	while(tmp != NULL) {
		printf("%d ", tmp->val);
		tmp = tmp->next;
	}
	printf("\n");
	pthread_mutex_unlock(&list_mutex);
}

struct list *add_node(struct list **head, int val)
{
	pthread_mutex_lock(&list_mutex);
	printf("Thread %ld is doing add_list\n", syscall(SYS_gettid));
	struct list* new_node = create_node(val);
	if (new_node == NULL)
		return NULL;

	if ((*head)->val == INT_MIN) {
		*head = new_node;
		pthread_mutex_unlock(&list_mutex);
		return *head;
	}

	struct list *tmp = *head;

	while(tmp->next != NULL)
		tmp = tmp->next;

	tmp->next = new_node;
	pthread_mutex_unlock(&list_mutex);

	return *head;

}

static void swap(struct list *a, struct list *b)
{
	if (a == NULL || b == NULL)
		return;

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
	printf("Thread %ld is doing sort_list\n", syscall(SYS_gettid));

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

	pthread_mutex_unlock(&list_mutex);

	return *head;
}

void flush_list(struct list **head)
{
	if (*head == NULL)
		return;

	while(*head) {
		struct list *tmp = *head;
		*head = (*head)->next;
		free(tmp);
	}
}

struct list *delete_node(struct list **head, int val)
{

	/* if the first node is the one
	 * that has to be deleted */

	pthread_mutex_lock(&list_mutex);
	printf("Thread %ld is doing delete_node\n", syscall(SYS_gettid));
	struct list *tmp = *head;
	struct list *prev = NULL;

	if (tmp != NULL && tmp->val == val)
	{
		*head = tmp->next;
		free(tmp);
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

		free(tmp);
	}

	pthread_mutex_unlock(&list_mutex);

	return *head;
}
