#include "OpenACC/utils/linked_list.h"

#include <stdlib.h>

linked_list_t
list_create ()
{
    linked_list_t list;
    list = (linked_list_t)	malloc(sizeof(struct linked_list_t_));
    list->data = node_create ();
    node_set_left_right (list->data, list->data, list->data);
    list->count = 0;
}

void
list_free (linked_list_t list)
{
    list_clear	(list);
    free(list->data);
    list->data = NULL;
    list->count = 0;
}

void
list_insert_front (linked_list_t list, node_t node)
{
    node_insert_after (list->data, node);
    list->count++;
}

void
list_emplace_front (linked_list_t list, void* val)
{
    node_t n;
    n = node_create ();
    n->data = val;
    list_insert_front (list, n);
}

void
list_insert_back (linked_list_t list, node_t node)
{
    node_insert_before (list->data, node);
    list->count++;
}

void
list_emplace_back (linked_list_t list, void* val)
{
    node_t n;
    n = node_create ();
    n->data = val;
    list_insert_back (list, n);
}

node_t
list_find (linked_list_t list, void* val, linked_list_comparator_t cmp)
{
    node_t curr;
    node_t head;
    head = list->data;
    curr = head->next;
    while (curr != head)
    {
	if (cmp (curr->data, val) == 0)
	    return curr;
	curr = curr->next;
    }
    return NULL;
}

void
list_remove_value (linked_list_t list , void* val, linked_list_comparator_t cmp)
{
    node_t curr;
    node_t head;
    head = list->data;
    curr = head->next;
    while (curr != head)
    {
	if (cmp (curr->data, val) == 0)
	{
	    // backtrack to avoid corruption
	    curr = curr->prev;
	    list_remove_node (list, curr->next);
	}
	curr = curr->next;
    }
}

void
list_remove_node (linked_list_t list, node_t node)
{
    if (node->next != node)
    {
	list->count--;
	node->next->prev = node->prev;
	node->prev->next = node->next;
	node_free(&node);
    }
}

size_t
list_count (linked_list_t list)
{
    return list->count;
}

unsigned char
list_empty (linked_list_t list)
{
    return (list->count == 0);
}

void
list_clear (linked_list_t list)
{
    while (list->data != list->data->next)
    {
	list_remove_node (list, list->data->next);
    }
}

