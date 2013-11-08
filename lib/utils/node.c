#include "OpenACC/utils/node.h"

#include <stddef.h>
#include <stdlib.h>

node_t
node_create ()
{
    node_t n;
    n = (node_t)malloc(sizeof(struct node_t_));
    n->prev = n;
    n->next = n;
    n->data = NULL;
    return n;
}

void
node_free (node_t * n)
{
    if ((*n)->prev != NULL)
	(*n)->prev->next = (*n)->next;
    if ((*n)->next != NULL)
	(*n)->next->prev = (*n)->prev;
    free (*n);
    *n = NULL;
}

void
node_insert_after (node_t node, node_t toInsert)
{
    node_set_left_right (toInsert, node, node->next);
}

void
node_insert_before (node_t node, node_t toInsert)
{
    node_set_left_right (toInsert, node->prev, node);
}

void
node_set_left (node_t node, node_t toLeft)
{
    node->prev = toLeft;
    if (toLeft != NULL)
	toLeft->next = node;
}

void
node_set_right (node_t node, node_t toRight)
{
    node->next = toRight;
    if (toRight != NULL)
	toRight->prev = node;
}

void
node_set_left_right (node_t node, node_t toLeft, node_t toRight)
{
    node_set_left (node, toLeft);
    node_set_right (node, toRight);    
}

