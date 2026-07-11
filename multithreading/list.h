#ifndef LIST_H
#define LIST_H

#include <stddef.h>

/**
 * struct node_s - node of a doubly linked list
 * @content: content of the node
 * @prev: pointer to previous node
 * @next: pointer to next node
 */
typedef struct node_s
{
	void *content;
	struct node_s *prev;
	struct node_s *next;
} node_t;

/**
 * struct list_s - doubly linked list
 * @size: number of nodes
 * @head: pointer to first node
 * @tail: pointer to last node
 */
typedef struct list_s
{
	size_t size;
	node_t *head;
	node_t *tail;
} list_t;

list_t *list_create(void);
node_t *list_add(list_t *list, void *content);
node_t *list_add_tail(list_t *list, void *content);
void list_destroy(list_t *list, void (*destroy)(void *));

#endif /* LIST_H */

list_t *list_init(list_t *list);
void list_each(list_t *list, void (*func)(void *));
