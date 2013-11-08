#ifndef _SORTED_LIST_H_
#define _SORTED_LIST_H_

#include <stddef.h>

typedef int(* sorted_list_comparator_t)(void*, void*);

struct
sorted_list_t_
{
  size_t capacity;
  size_t count;
  size_t element_size;
  void** data;
  sorted_list_comparator_t comp;
};

typedef struct sorted_list_t_* sorted_list_t;

sorted_list_t
sorted_list_create (sorted_list_comparator_t);

void
sorted_list_free (sorted_list_t*);

void
sorted_list_insert (sorted_list_t, void*);

void
sorted_list_remove (sorted_list_t, void*);

void
sorted_list_clear (sorted_list_t);

size_t
sorted_list_index_of (sorted_list_t, void*);

void*
sorted_list_get (sorted_list_t, size_t);

/* internal */

void
sorted_list_grow (sorted_list_t);

void
sorted_list_shrink (sorted_list_t);

void
sorted_list_shift_left (sorted_list_t, size_t, size_t);

void
sorted_list_shift_right (sorted_list_t, size_t, size_t);

#endif /* _SORTED_ARRAY_H_ */
