#include "container.h"

#include <linux/slab.h>
#include <linux/string.h>

//////////////////////////////////////////////////////////////////////
// vlist

struct vlist_node
{
	void* data;
	struct vlist_node* next;
	struct vlist_node* prev;
};

struct vlist
{
	vlist_node* first;
	vlist_node* last;
	vlist_node* curr;
};

vlist* vlist_new(void)
{
	vlist* new_list = (vlist*)kmalloc(sizeof(vlist), GFP_KERNEL);
	vlist_init(new_list);
	return new_list;
}

void vlist_init(vlist* list)
{
	if (!list)
		return;

	list->first = NULL;
	list->last = NULL;
	list->curr = NULL;
}

void vlist_push(vlist* list, void* data)
{
	vlist_node* node = (vlist_node*)kmalloc(sizeof(vlist_node), GFP_KERNEL);
	node->data = data;

	if (list->first == NULL)
	{
		list->first = node;
		node->next = NULL;
		node->prev = NULL;
	}
	else
	{
		node->prev = list->last;
		node->prev->next = node;
		node->next = NULL;
	}

	list->curr = node;
	list->last = node;
}

void vlist_push_copy(vlist* list, void* data, size_t size)
{
	void* copy = kmalloc(size, GFP_KERNEL);
	memcpy(copy, data, size);
	vlist_push(list, copy);
}

BOOL vlist_find(vlist* list, const void* data, size_t size)
{
	vlist_node* node = list->first;
	if (!node)
		return FALSE;

	while (node)
	{
		if (memcmp(node->data, data, size) == 0)
		{
			list->curr = node;
			return TRUE;
		}

		node = node->next;
	}

	return FALSE;
}

BOOL vlist_first(vlist* list)
{
	if (!list->first)
		return FALSE;

	list->curr = list->first;
	return TRUE;
}

BOOL vlist_last(vlist* list)
{
	if (!list->last)
		return FALSE;

	list->curr = list->last;
	return TRUE;
}

BOOL vlist_next(vlist* list)
{
	if (!list->curr || !list->curr->next)
	{
		list->curr = NULL;
		return FALSE;
	}

	list->curr = list->curr->next;
	return TRUE;
}

BOOL vlist_prev(vlist* list)
{
	if (!list->curr || !list->curr->prev)
	{
		list->curr = NULL;
		return FALSE;
	}

	list->curr = list->curr->prev;
	return TRUE;
}

BOOL vlist_end(vlist* list)
{
	return list->curr == NULL;
}

void* vlist_get(vlist* list)
{
	if (!list->curr)
		return NULL;

	return list->curr->data;
}

void vlist_remove(vlist* list, BOOL delete)
{
	if (!list->curr)
		return;

	vlist_node* curr = list->curr;

	if (list->curr->prev)
	{
		list->curr = list->curr->prev;
		if (curr->next)
		{
			list->curr->next = curr->next;
			curr->next->prev = list->curr;
		}
		else
			list->curr->next = NULL;
	}
	else if (list->curr->next)
	{
		list->curr = list->curr->next;
		if (curr->prev)
			list->curr->prev = curr->prev;
		else
			list->curr->prev = NULL;
	}

	if (curr == list->first && curr == list->last)
	{
		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;
	}
	else
	{
		if (curr == list->first)
			list->first = list->curr;

		if (curr == list->last)
			list->last = list->curr;
	}

	if (delete && curr->data)
		kfree(curr->data);
	kfree(curr);
}

size_t vlist_size(vlist* list)
{
	vlist_node* node = list->first;
	if (!node)
		return 0;

	size_t count = 0;
	while (node)
	{
		count++;
		node = node->next;
	}

	return count;
}

BOOL vlist_empty(vlist* list)
{
	return list->first == NULL;
}

void vlist_clear(vlist* list, BOOL delete)
{
	vlist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		vlist_node* temp = node->next;
		if (delete && node->data)
			kfree(node->data);
		kfree(node);
		node = temp;
	}

	vlist_init(list);
}

void vlist_delete(vlist* list, BOOL delete)
{
	vlist_clear(list, delete);
	kfree(list);
}

void vlist_append(vlist* dest, vlist* src)
{
	vlist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		vlist_push(dest, src_node->data);
		src_node = src_node->next;
	}
}

void vlist_print(vlist* list)
{
	vlist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		pr_info("Netfilter: %s\n", (char*)node->data);
		node = node->next;
	}
}


//////////////////////////////////////////////////////////////////////
// nlist

struct nlist_node
{
	unsigned int data;
	struct nlist_node* next;
	struct nlist_node* prev;
};

struct nlist
{
	nlist_node* first;
	nlist_node* last;
	nlist_node* curr;
};

nlist* nlist_new(void)
{
	nlist* list = (nlist*)kmalloc(sizeof(nlist), GFP_KERNEL);
	nlist_init(list);
	return list;
}

void nlist_init(nlist* list)
{
	if (!list)
		return;

	list->first = NULL;
	list->last = NULL;
	list->curr = NULL;
}

void nlist_push(nlist* list, unsigned int data)
{
	nlist_node* node = (nlist_node*)kmalloc(sizeof(nlist_node), GFP_KERNEL);
	node->data = data;

	if (list->first == NULL)
	{
		list->first = node;
		node->next = NULL;
		node->prev = NULL;
	}
	else
	{
		node->prev = list->last;
		node->prev->next = node;
		node->next = NULL;
	}

	list->curr = node;
	list->last = node;
}

BOOL nlist_find(nlist* list, unsigned int data)
{
	nlist_node* node = list->first;
	if (!node)
		return FALSE;

	while (node)
	{
		if (node->data == data)
		{
			list->curr = node;
			return TRUE;
		}

		node = node->next;
	}

	return FALSE;
}

BOOL nlist_first(nlist* list)
{
	if (!list->first)
		return FALSE;

	list->curr = list->first;
	return TRUE;
}

BOOL nlist_last(nlist* list)
{
	if (!list->last)
		return FALSE;

	list->curr = list->last;
	return TRUE;
}

BOOL nlist_next(nlist* list)
{
	if (!list->curr->next)
	{
		list->curr = NULL;
		return FALSE;
	}

	list->curr = list->curr->next;
	return TRUE;
}

BOOL nlist_prev(nlist* list)
{
	if (!list->curr->prev)
		return FALSE;

	list->curr = list->curr->prev;
	return TRUE;
}

BOOL nlist_end(nlist* list)
{
	return list->curr == NULL;
}

unsigned int nlist_get(nlist* list)
{
	if (!list->curr)
		return 0;

	return list->curr->data;
}

void nlist_remove(nlist* list)
{
	if (!list->curr)
		return;

	nlist_node* curr = list->curr;

	if (list->curr->prev)
	{
		list->curr = list->curr->prev;
		if (curr->next)
		{
			list->curr->next = curr->next;
			curr->next->prev = list->curr;
		}
		else
			list->curr->next = NULL;
	}
	else if (list->curr->next)
	{
		list->curr = list->curr->next;
		if (curr->prev)
			list->curr->prev = curr->prev;
		else
			list->curr->prev = NULL;
	}

	if (curr == list->first && curr == list->last)
	{
		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;
	}
	else
	{
		if (curr == list->first)
			list->first = list->curr;

		if (curr == list->last)
			list->last = list->curr;
	}

	kfree(curr);
}

size_t nlist_size(nlist* list)
{
	nlist_node* node = list->first;
	if (!node)
		return 0;

	size_t count = 0;
	while (node)
	{
		count++;
		node = node->next;
	}

	return count;
}

BOOL nlist_empty(nlist* list)
{
	return list->first == NULL;
}

void nlist_clear(nlist* list)
{
	nlist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		nlist_node* temp = node->next;
		kfree(node);
		node = temp;
	}

	nlist_init(list);
}

void nlist_delete(nlist* list)
{
	nlist_clear(list);
	kfree(list);
}

void nlist_append(nlist* dest, nlist* src)
{
	nlist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		nlist_push(dest, src_node->data);
		src_node = src_node->next;
	}
}

void nlist_append_unique(nlist* dest, nlist* src)
{
	nlist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		if (!nlist_find(dest, src_node->data))
			nlist_push(dest, src_node->data);
		src_node = src_node->next;
	}
}

void nlist_remove_list(nlist* dest, nlist* src)
{
	nlist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		if (nlist_find(dest, src_node->data))
			nlist_remove(dest);
		src_node = src_node->next;
	}
}

void nlist_print(nlist* list)
{
	nlist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		pr_info("Netfilter: %u\n", node->data);
		node = node->next;
	}
}

void nlist_print_ip(nlist* list)
{
	nlist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		char s_ip[16] = {0};
		sprintf(s_ip, "%u.%u.%u.%u"
			, (node->data) & 0xff
			, (node->data >> 8) & 0xff
			, (node->data >> 16) & 0xff
			, (node->data >> 24) & 0xff);
		pr_info("Netfilter: %s\n", s_ip);
		node = node->next;
	}
}

//////////////////////////////////////////////////////////////////////
// slist

struct slist_node
{
	char* data;
	struct slist_node* next;
	struct slist_node* prev;
};

struct slist
{
	slist_node* first;
	slist_node* last;
	slist_node* curr;
};

slist* slist_new(void)
{
	slist* list = (slist*)kmalloc(sizeof(slist), GFP_KERNEL);
	slist_init(list);
	return list;
}

void slist_init(slist* list)
{
	if (!list)
		return;

	list->first = NULL;
	list->last = NULL;
	list->curr = NULL;
}

void slist_push(slist* list, char* data)
{
	slist_node* node = (slist_node*)kmalloc(sizeof(slist_node), GFP_KERNEL);
	node->data = data;

	if (list->first == NULL)
	{
		list->first = node;
		node->next = NULL;
		node->prev = NULL;
	}
	else
	{
		node->prev = list->last;
		node->prev->next = node;
		node->next = NULL;
	}

	list->curr = node;
	list->last = node;
}

void slist_push_copy(slist* list, const char* data)
{
	char* copy = kstrdup(data, GFP_KERNEL);
	slist_push(list, copy);
}

BOOL slist_find(slist* list, const char* data)
{
	slist_node* node = list->first;
	if (!node)
		return FALSE;

	while (node)
	{
		if (strcmp(node->data, data) == 0)
		{
			list->curr = node;
			return TRUE;
		}

		node = node->next;
	}

	return FALSE;
}

BOOL slist_find_part(slist* list, const char* data)
{
	slist_node* node = list->first;
	if (!node)
		return FALSE;

	while (node)
	{
		if (strstr(data, node->data))
		{
			list->curr = node;
			return TRUE;
		}

		node = node->next;
	}

	return FALSE;
}

BOOL slist_first(slist* list)
{
	if (!list->first)
		return FALSE;

	list->curr = list->first;
	return TRUE;
}

BOOL slist_last(slist* list)
{
	if (!list->last)
		return FALSE;

	list->curr = list->last;
	return TRUE;
}

BOOL slist_next(slist* list)
{
	if (!list->curr || !list->curr->next)
	{
		list->curr = NULL;
		return FALSE;
	}

	list->curr = list->curr->next;
	return TRUE;
}

BOOL slist_prev(slist* list)
{
	if (!list->curr || !list->curr->prev)
		return FALSE;

	list->curr = list->curr->prev;
	return TRUE;
}

BOOL slist_end(slist* list)
{
	return list->curr == NULL;
}

char* slist_get(slist* list)
{
	if (!list->curr)
		return NULL;

	return list->curr->data;
}

void slist_remove(slist* list, BOOL clean)
{
	if (!list->curr)
		return;

	slist_node* curr = list->curr;

	if (list->curr->prev)
	{
		list->curr = list->curr->prev;
		if (curr->next)
		{
			list->curr->next = curr->next;
			curr->next->prev = list->curr;
		}
		else
			list->curr->next = NULL;
	}
	else if (list->curr->next)
	{
		list->curr = list->curr->next;
		if (curr->prev)
			list->curr->prev = curr->prev;
		else
			list->curr->prev = NULL;
	}

	if (curr == list->first && curr == list->last)
	{
		list->first = NULL;
		list->last = NULL;
		list->curr = NULL;
	}
	else
	{
		if (curr == list->first)
			list->first = list->curr;

		if (curr == list->last)
			list->last = list->curr;
	}

	if (clean && curr->data)
		kfree(curr->data);
	kfree(curr);
}

size_t slist_size(slist* list)
{
	slist_node* node = list->first;
	if (!node)
		return 0;

	size_t count = 0;
	while (node)
	{
		count++;
		node = node->next;
	}

	return count;
}

BOOL slist_empty(slist* list)
{
	return list->first == NULL;
}

void slist_clear(slist* list, BOOL clean)
{
	slist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		slist_node* temp = node->next;
		if (clean && node->data)
			kfree(node->data);
		kfree(node);
		node = temp;
	}

	slist_init(list);
}

void slist_delete(slist* list, BOOL clean)
{
	slist_clear(list, clean);
	kfree(list);
}

void slist_append(slist* dest, slist* src)
{
	slist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		slist_push_copy(dest, src_node->data);
		src_node = src_node->next;
	}
}

void slist_append_unique(slist* dest, slist* src)
{
	slist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		if (!slist_find(dest, src_node->data))
			slist_push_copy(dest, src_node->data);
		src_node = src_node->next;
	}
}

void slist_remove_list(slist* dest, slist* src)
{
	slist_node* src_node = src->first;
	if (!src_node)
		return;

	while (src_node)
	{
		if (slist_find(dest, src_node->data))
			slist_remove(dest, TRUE);
		src_node = src_node->next;
	}
}

void slist_print(slist* list)
{
	slist_node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		pr_info("Netfilter: %s\n", node->data);
		node = node->next;
	}
}

//////////////////////////////////////////////////////////////////////
// nset

struct nset_node
{
	long key;
	vlist* nodes;
	BOOL removed;
};

struct nset
{
	size_t size;
	size_t count;
	size_t node_count;
	nset_node* nodes;
	vlist* iterator;
};

static size_t nset_hash(long key)
{
	return key;
}

static inline size_t nset_index(nset* set, long key)
{
	size_t hash = nset_hash(key);
	size_t index = hash % set->size;
	return index;
}

static struct nset_node* nset_insert_node(nset* set, long key)
{
	size_t index = nset_index(set, key);

	// Check collision
	if (set->nodes[index].key != KEY_NULL)
	{
		// Check the same key
		if (set->nodes[index].key == key)
		{
			return &set->nodes[index];
		}
		// Collision
		else
		{
			nset_node* node = (nset_node*)kmalloc(sizeof(nset_node), GFP_KERNEL);
			node->key = key;
			node->nodes = NULL;
			node->removed = FALSE;

			vlist* nodes = set->nodes[index].nodes;
			if (nodes == NULL)
			{
				nodes = vlist_new();
				set->nodes[index].nodes = nodes;
			}

			vlist_push(nodes, node);
			vlist_push(set->iterator, node);
			set->node_count++;
			return node;
		}
	}

	set->nodes[index].key = key;
	set->nodes[index].removed = FALSE;
	set->node_count++;
	vlist_push(set->iterator, &set->nodes[index]);
	return &set->nodes[index];
}

static struct nset_node* nset_find_list(vlist* nodes, long key)
{
	for (vlist_first(nodes); !vlist_end(nodes); vlist_next(nodes))
	{
		nset_node* node = (nset_node*)vlist_get(nodes);
		if (node == NULL || node->key == KEY_NULL)
			return NULL;

		if (node->key == key)
			return node;
	}

	return NULL;
}

static void nset_rehash(nset* set, size_t size)
{
	nset_node* old_nodes = set->nodes;

	if (size == 0)
		set->size *= 2;
	else
		set->size = size;

	set->nodes = kcalloc(set->size, sizeof(struct nset_node), GFP_KERNEL);
	set->node_count = 0;

	// Clear iterator list
	for (vlist_first(set->iterator); !vlist_end(set->iterator); vlist_next(set->iterator))
	{
		nset_node* node = (nset_node*)vlist_get(set->iterator);
		if (node->removed)
			vlist_remove(set->iterator, TRUE);
	}
	vlist_clear(set->iterator, FALSE);

	for (size_t i = 0; i < set->size; ++i)
		set->nodes[i].key = KEY_NULL;

	for (size_t i = 0; i < set->count; ++i)
	{
		vlist* nodes = old_nodes[i].nodes;
		if (nodes != NULL)
		{
			for (vlist_first(nodes); !vlist_end(nodes); vlist_next(nodes))
			{
				nset_node* node = (nset_node*)vlist_get(nodes);
				if (node->key == KEY_NULL)
					continue;

				nset_insert_node(set, node->key);
			}

			vlist_delete(nodes, TRUE);
		}

		if (old_nodes[i].key == KEY_NULL)
			continue;

		nset_insert_node(set, old_nodes[i].key);
	}

	kfree(old_nodes);
}

nset* nset_new()
{
	nset* set = (nset*)kcalloc(1, sizeof(nset), GFP_KERNEL);
	nset_init(set);
	return set;
}

void nset_reserve(nset* set, size_t size)
{
	if (size < set->size)
		return;

	set->size = size;
	nset_rehash(set, size);
}

void nset_init(nset* set)
{
	set->count = 0;
	set->node_count = 0;
	set->size = NSET_INCR_SIZE;

	if (set->iterator == NULL)
		set->iterator = vlist_new();
	else
		vlist_clear(set->iterator, TRUE);

	if (set->nodes != NULL)
		kfree(set->nodes);

	set->nodes = (nset_node*)kcalloc(set->size, sizeof(nset_node), GFP_KERNEL);

	for (size_t i = 0; i < set->size; ++i)
		set->nodes[i].key = KEY_NULL;
}

void nset_clear(nset* set)
{
	if (set->nodes != NULL)
	{
		for (size_t i = 0; i < set->size; ++i)
		{
			vlist* nodes = set->nodes[i].nodes;
			if (nodes != NULL)
			{
				vlist_delete(nodes, TRUE);
				set->nodes[i].nodes = NULL;
			}
		}

		kfree(set->nodes);
		set->nodes = NULL;
	}

	set->size = 0;
	set->count = 0;
	set->node_count = 0;

	if (set->iterator != NULL)
		vlist_clear(set->iterator, FALSE);
}

void nset_delete(nset* set)
{
	nset_clear(set);
	vlist_delete(set->iterator, TRUE);
	kfree(set);
}

void nset_insert(nset* set, long key)
{
	nset_insert_node(set, key);
	set->count++;

	if (set->count >= set->size)
		nset_rehash(set, 0);
}

void nset_remove(nset* set, long key)
{
	size_t index = nset_index(set, key);

	vlist* nodes = set->nodes[index].nodes;
	if (nodes != NULL)
	{
		nset_node* node = nset_find_list(nodes, key);
		if (node != NULL)
		{
			// Don't free the nset_node, this will be removed in iterator list
			vlist_remove(nodes, FALSE);
			node->removed = TRUE;
			set->node_count--;
			return;
		}
	}

	if (set->nodes[index].key == key)
	{
		set->nodes[index].key = KEY_NULL;
		set->nodes[index].removed = TRUE;
		set->count--;
		set->node_count--;
	}
}

BOOL nset_find(nset* set, long key)
{
	if (set->node_count == 0 || set->count == 0)
		return FALSE;

	size_t index = nset_index(set, key);

	if (set->nodes[index].key != KEY_NULL &&
		set->nodes[index].key == key)
		return TRUE;

	vlist* nodes = set->nodes[index].nodes;
	if (nodes != NULL)
	{
		nset_node* node = nset_find_list(nodes, key);
		if (node != NULL)
			return TRUE;
	}

	return FALSE;
}

size_t nset_size(nset* set)
{
	return set->node_count;
}

BOOL nset_empty(nset* set)
{
	return set->node_count == 0;
}

BOOL nset_first(nset* set)
{
	return vlist_first(set->iterator);
}

BOOL nset_last(nset* set)
{
	return vlist_last(set->iterator);
}

BOOL nset_next(nset* set)
{
	if (!vlist_next(set->iterator))
		return FALSE;

	nset_node* node = vlist_get(set->iterator);
	while (node && node->removed)
	{
		vlist_next(set->iterator);
		node = vlist_get(set->iterator);
	}

	return (node ? TRUE : FALSE);
}

BOOL nset_prev(nset* set)
{
	if (!vlist_prev(set->iterator))
		return FALSE;

	nset_node* node = vlist_get(set->iterator);
	while (node && node->removed)
	{
		vlist_prev(set->iterator);
		node = vlist_get(set->iterator);
	}

	return (node ? TRUE : FALSE);
}

long nset_get(nset* set)
{
	nset_node* node = vlist_get(set->iterator);
	if (node == NULL)
		return KEY_NULL;

	while (node && node->removed)
	{
		vlist_next(set->iterator);
		node = vlist_get(set->iterator);
	}

	return (node ? node->key : KEY_NULL);
}

void nset_for_each(nset* set, nset_get_value get_value)
{
	for (size_t i = 0; i < set->size; ++i)
	{
		if (set->nodes[i].key != KEY_NULL)
			get_value(set->nodes[i].key);

		vlist* nodes = set->nodes[i].nodes;
		if (nodes != NULL)
		{
			for (vlist_first(nodes); !vlist_end(nodes); vlist_next(nodes))
			{
				nset_node* node = (nset_node*)vlist_get(nodes);
				if (node != NULL && node->key != KEY_NULL)
					get_value(node->key);
			}
		}
	}
}

void nset_append(nset* set, nset* src)
{
	size_t new_size = set->node_count + src->node_count;
	if (new_size > set->size)
		nset_reserve(set, new_size);

	nset_first(src);
	do
	{
		long key = nset_get(src);
		if (key != KEY_NULL)
			nset_insert(set, key);
	}
	while (nset_next(src));
}

void nset_append_list(nset* set, nlist* src)
{
	size_t new_size = set->node_count + nlist_size(src);
	if (new_size > set->size)
		nset_reserve(set, new_size);

	for (nlist_first(src); !nlist_end(src); nlist_next(src))
	{
		long key = nlist_get(src);
		if (key != KEY_NULL)
			nset_insert(set, key);
	}
}

void nset_remove_list(nset* set, nlist* src)
{
	for (nlist_first(src); !nlist_end(src); nlist_next(src))
	{
		long key = nlist_get(src);
		if (key != KEY_NULL)
			nset_remove(set, key);
	}
}

void nset_print(nset*set)
{
	for (vlist_first(set->iterator); !vlist_end(set->iterator); vlist_next(set->iterator))
	{
		nset_node* node = (nset_node*)vlist_get(set->iterator);
		if (node->removed || node->key == KEY_NULL)
			continue;

		pr_info("%ld\n", node->key);
	}
}

void nset_print_ip(nset* set)
{
	for (vlist_first(set->iterator); !vlist_end(set->iterator); vlist_next(set->iterator))
	{
		nset_node* node = (nset_node*)vlist_get(set->iterator);
		if (node->removed || node->key == KEY_NULL)
			continue;

		unsigned int ip = (unsigned int)node->key;
		char s_ip[16] = {0};
		sprintf(s_ip, "%u.%u.%u.%u"
			, (ip) & 0xff
			, (ip >> 8) & 0xff
			, (ip >> 16) & 0xff
			, (ip >> 24) & 0xff);
		pr_info("Netfilter: %s\n", s_ip);
	}
}
