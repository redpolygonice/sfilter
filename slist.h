#ifndef SLIST_H
#define SLIST_H

#include "types.h"

typedef struct slist_node
{
	char* data;
	struct slist_node* next;
	struct slist_node* prev;
} slist_node;

typedef struct slist
{
	slist_node* first;
	slist_node* last;
	slist_node* curr;
} slist;

slist* slist_new(void);
void slist_init(slist* slist);
void slist_push(slist* list, char* data);
void slist_push_copy(slist* list, const char* data);
BOOL slist_find(slist* list, const char* data);
BOOL slist_find_part(slist* list, const char* data);
BOOL slist_first(slist* list);
BOOL slist_last(slist* list);
BOOL slist_next(slist* list);
BOOL slist_prev(slist* list);
BOOL slist_end(slist* list);
char* slist_get(slist* list);
void slist_remove(slist* list, BOOL delete);
int slist_count(slist* list);
BOOL slist_empty(slist* list);
void slist_clear(slist* list, BOOL delete);
void slist_delete(slist* list, BOOL delete);
void slist_append(slist* dest, slist* src);
void slist_append_unique(slist* dest, slist* src);
void slist_remove_list(slist* dest, slist* src);
void slist_print(slist* list);


#endif // SLIST_H
