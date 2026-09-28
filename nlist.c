#include "nlist.h"

#include <linux/slab.h>
#include <linux/string.h>

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

int nlist_count(nlist* list)
{
	nlist_node* node = list->first;
	if (!node)
		return 0;

	int count = 0;
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
	list = NULL;
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
