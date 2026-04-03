#ifndef NLIST_H
#define NLIST_H

#include "types.h"

typedef struct NNode
{
	unsigned int data;
	struct NNode* next;
	struct NNode* prev;
} NNode;

typedef struct nlist
{
	NNode* first;
	NNode* last;
	NNode* curr;
} nlist;

nlist* nlist_new(void);
void nlist_init(nlist* list);
void nlist_push(nlist* list, unsigned int data);
BOOL nlist_find(nlist* list, unsigned int data);
BOOL nlist_first(nlist* list);
BOOL nlist_last(nlist* list);
BOOL nlist_next(nlist* list);
BOOL nlist_prev(nlist* list);
BOOL nlist_end(nlist* list);
unsigned int nlist_get(nlist* list);
void nlist_remove(nlist* list);
int nlist_count(nlist* list);
BOOL nlist_empty(nlist* list);
void nlist_clear(nlist* list);
void nlist_delete(nlist* list);
void nlist_append(nlist* dest, nlist* src);
void nlist_append_unique(nlist* dest, nlist* src);
void nlist_remove_list(nlist* dest, nlist* src);
void nlist_print(nlist* list);
void nlist_print_ip(nlist* list);


#endif // NLIST_H
