#include "slist.h"

#include <linux/slab.h>
#include <linux/string.h>

slist* slist_new(void)
{
	slist* list = (slist*)kmalloc(sizeof(slist), GFP_KERNEL);
	slist_init(list);
	return list;
}

void slist_init(slist* slist)
{
	if (!slist)
		return;

	slist->first = NULL;
	slist->last = NULL;
	slist->curr = NULL;
}

void slist_push(slist* list, char* data)
{
	Node* node = (Node*)kmalloc(sizeof(Node), GFP_KERNEL);
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
	Node* node = list->first;
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
	Node* node = list->first;
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

void slist_remove(slist* list, BOOL delete)
{
	if (!list->curr)
		return;

	Node* curr = list->curr;

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

int slist_count(slist* list)
{
	Node* node = list->first;
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

BOOL slist_empty(slist* list)
{
	return list->first == NULL;
}

void slist_clear(slist* list, BOOL delete)
{
	Node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		Node* temp = node->next;
		if (delete && node->data)
			kfree(node->data);
		kfree(node);
		node = temp;
	}

	slist_init(list);
}

void slist_delete(slist* list, BOOL delete)
{
	slist_clear(list, delete);
	kfree(list);
}

void slist_append(slist* dest, slist* src)
{
	Node* src_node = src->first;
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
	Node* src_node = src->first;
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
	Node* src_node = src->first;
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
	Node* node = list->first;
	if (!node)
		return;

	while (node)
	{
		pr_info("Netfilter: %s\n", node->data);
		node = node->next;
	}
}
