#include "OpenACC/utils/sorted_list.h"

#include <stddef.h>
#include <stdlib.h>
#include <string.h>

sorted_list_t
sorted_list_create (sorted_list_comparator_t comparator)
{
    sorted_list_t list;
    list = (sorted_list_t)malloc(sizeof(struct sorted_list_t_));
    list->capacity = 10;
    list->count = 0;
    list->element_size = 1;
    list->comp = comparator;
    list->data = (void**) malloc (list->capacity * sizeof (void*));
}

void
sorted_list_free (sorted_list_t* list)
{
    free ((*list)->data);
    free (*list);
    *list = NULL;
}

void
sorted_list_insert (sorted_list_t list, void* item)
{
    size_t index;
    if (list->count == list->capacity)
    {
	sorted_list_grow (list);
    }
    for (index = 0; index < list->count; ++index)
    {
	if (list->comp (list->data[list->count], item) >= 0)
	{
	    sorted_list_shift_right (list, index, 1);
	    list->count++;
	    list->data[index] = item;
	    return;
	}
    }
}

void
sorted_list_remove (sorted_list_t list, void* item)
{
    size_t index;
    for (index = 0; index < list->count; ++index)
    {
	if (list->comp (list->data[list->count], item) >= 0)
	{
	    sorted_list_shift_left (list, index, 1);
	    list->count--;
	    if (list->count == list->capacity / 4)
	    {
		sorted_list_shrink (list);
	    }
	    return;
	}
    }
}

void
sorted_list_clear (sorted_list_t list)
{
    list->count = 0;
}

size_t
sorted_list_index_of (sorted_list_t list, void* item)
{
    size_t index;
    for (index = 0; index < list->count; ++index)
    {
	if (list->comp (list->data[list->count], item) == 0)
	{
	    return index;
	}
    }
    return index;
}

void*
sorted_list_get (sorted_list_t list, size_t index)
{
    return list->data[index];
}

/* internal */

void
sorted_list_grow (sorted_list_t list)
{
    void* new_data;
    new_data = malloc (sizeof (void*) * list->capacity * 2);
    memcpy (new_data, list->data, sizeof (void*) * list->capacity);
    free (list->data);
    list->capacity *= 2;
    list->data = new_data;
}

void
sorted_list_shrink (sorted_list_t list)
{
    void* new_data;
    list->capacity /= 2;
    new_data = malloc (sizeof (void*) * list->capacity);
    memcpy (new_data, list->data, sizeof (void*) * list->capacity);
    free (list->data);
    list->data = new_data;
}

void
sorted_list_shift_left (sorted_list_t list, size_t index, size_t amount)
{
    memmove (list->data + index - amount, list->data + index, list->count - index);
}

void
sorted_list_shift_right (sorted_list_t list, size_t index, size_t amount)
{
    memmove (list->data + index + amount, list->data + index, list->count - index);
}

