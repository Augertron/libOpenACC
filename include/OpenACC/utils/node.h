#ifndef _NODE_H_
#define _NODE_H_

typedef struct node_t_ * node_t;

struct
node_t_
{
    node_t prev;
    node_t next;
    void* data;
};

node_t
node_create ();

void
node_free (node_t *);

void
node_insert_after (node_t, node_t);

void
node_insert_before (node_t, node_t);

void
node_set_left (node_t, node_t);

void
node_set_right (node_t, node_t);

void
node_set_left_right (node_t, node_t, node_t);

#endif /* _NODE_H_ */
