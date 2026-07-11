#include <stdlib.h>
#include "list.h"

/**
 * list_create - creates a new list
 *
 * Return: pointer to new list, or NULL on failure
 */
list_t *list_create(void)
{
	list_t *list;

	list = malloc(sizeof(*list));
	if (!list)
		return (NULL);
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
	return (list);
}

/**
 * list_add - adds node at head of list
 * @list: pointer to list
 * @content: content to store
 *
 * Return: pointer to new node, or NULL on failure
 */
node_t *list_add(list_t *list, void *content)
{
	node_t *node;

	node = malloc(sizeof(*node));
	if (!node)
		return (NULL);
	node->content = content;
	node->prev = NULL;
	node->next = list->head;
	if (list->head)
		list->head->prev = node;
	else
		list->tail = node;
	list->head = node;
	list->size++;
	return (node);
}

/**
 * list_add_tail - adds node at tail of list
 * @list: pointer to list
 * @content: content to store
 *
 * Return: pointer to new node, or NULL on failure
 */
node_t *list_add_tail(list_t *list, void *content)
{
	node_t *node;

	node = malloc(sizeof(*node));
	if (!node)
		return (NULL);
	node->content = content;
	node->next = NULL;
	node->prev = list->tail;
	if (list->tail)
		list->tail->next = node;
	else
		list->head = node;
	list->tail = node;
	list->size++;
	return (node);
}

/**
 * list_destroy - destroys a list
 * @list: pointer to list
 * @destroy: function to destroy content
 */
void list_destroy(list_t *list, void (*destroy)(void *))
{
	node_t *node;
	node_t *next;

	node = list->head;
	while (node)
	{
		next = node->next;
		if (destroy)
			destroy(node->content);
		free(node);
		node = next;
	}
	list->size = 0;
	list->head = NULL;
	list->tail = NULL;
}
