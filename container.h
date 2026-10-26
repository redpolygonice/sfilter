#ifndef CONTAINER_H
#define CONTAINER_H

#include "types.h"

//////////////////////////////////////////////////////////////////////
// vlist

typedef struct vlist_node vlist_node;
typedef struct vlist vlist;

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

//////////////////////////////////////////////////////////////////////
// nlist

typedef struct nlist_node nlist_node;
typedef struct nlist nlist;

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
size_t nlist_size(nlist* list);
BOOL nlist_empty(nlist* list);
void nlist_clear(nlist* list);
void nlist_delete(nlist* list);
void nlist_append(nlist* dest, nlist* src);
void nlist_append_unique(nlist* dest, nlist* src);
void nlist_remove_list(nlist* dest, nlist* src);
void nlist_print(nlist* list);
void nlist_print_ip(nlist* list);

//////////////////////////////////////////////////////////////////////
// slist

typedef struct slist_node slist_node;
typedef struct slist slist;

slist* slist_new(void);
void slist_init(slist* list);
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
void slist_remove(slist* list, BOOL clean);
size_t slist_size(slist* list);
BOOL slist_empty(slist* list);
void slist_clear(slist* list, BOOL clean);
void slist_delete(slist* list, BOOL clean);
void slist_append(slist* dest, slist* src);
void slist_append_unique(slist* dest, slist* src);
void slist_remove_list(slist* dest, slist* src);
void slist_print(slist* list);

//////////////////////////////////////////////////////////////////////
// nset

typedef void (*nset_get_value)(long);

#define NSET_INCR_SIZE 16

typedef struct nset_node nset_node;
typedef struct nset nset;

nset* nset_new(void);
void nset_reserve(nset* set, size_t size);
void nset_init(nset* set);
void nset_clear(nset* set);
void nset_delete(nset* set);
void nset_insert(nset* set, long key);
void nset_remove(nset* set, long key);
BOOL nset_find(nset* set, long key);
void nset_append(nset* set, nset* src);
void nset_append_list(nset* set, nlist* src);
void nset_remove_list(nset* set, nlist* src);
size_t nset_size(nset* set);
BOOL nset_empty(nset* set);
BOOL nset_first(nset* set);
BOOL nset_last(nset* set);
BOOL nset_next(nset* set);
BOOL nset_prev(nset* set);
long nset_get(nset* set);
void nset_for_each(nset* set, nset_get_value get_value);
void nset_print(nset* set);
void nset_print_ip(nset* set);

#endif // CONTAINER_H
