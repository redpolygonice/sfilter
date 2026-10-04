#include "vlist.h"

#include <linux/slab.h>
#include <linux/string.h>

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
		return FALSE;

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
	list = NULL;
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
