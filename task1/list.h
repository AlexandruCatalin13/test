#ifndef __LIST_H__
#define __LIST_H__

struct list *add_node(struct list **, int);
struct list *delete_node(struct list **, int);
void print_list(struct list *);
struct list *sort_list(struct list **);
void flush_list(struct list **);
void __print_val(struct list *);

struct list
{
	int val;
	void (*print_val)(struct list *);
	struct list *next;
};

#endif
