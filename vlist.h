#ifndef VLIST_H
#define VLIST_H

#include "types.h"

typedef struct list_node
{
	void* data;
	struct list_node* next;
	struct list_node* prev;
} vlist_node;

typedef struct list
{
	vlist_node* first;
	vlist_node* last;
	vlist_node* curr;
} vlist;

vlist* vlist_new(void);
void vlist_init(vlist* list);
void vlist_push(vlist* list, void* data);
void vlist_push_copy(vlist* list, void* data, size_t size);
BOOL vlist_find(vlist* list, const void* data, size_t size);
BOOL vlist_first(vlist* list);
BOOL vlist_last(vlist*list);
BOOL vlist_next(vlist* list);
BOOL vlist_prev(vlist*list);
BOOL vlist_end(vlist* list);
void* vlist_get(vlist*  list);
void vlist_remove(vlist* list, BOOL delete);
size_t vlist_size(vlist* list);
BOOL vlist_empty(vlist*list);
void vlist_clear(vlist* list, BOOL delete);
void vlist_delete(vlist* list, BOOL delete);
void vlist_append(vlist* dest, vlist* src);
void vlist_print(vlist* list);


#endif // VLIST_H
