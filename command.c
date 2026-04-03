#include "command.h"

#include <linux/slab.h>
#include <linux/string.h>

static slist* split_string(const char* text, const char* sep)
{
	slist* list = slist_new();

	char *copy = kstrdup(text, GFP_KERNEL);
	char *temp = copy;
	char *token = strsep(&copy, sep);

	while (token != NULL)
	{
		slist_push(list, kstrdup(token, GFP_KERNEL));
		token = strsep(&copy, sep);
	}

	kfree(temp);
	return list;
}

static unsigned int ipstr_to_uint(const char* s_ip)
{
	slist* ip_list = split_string(s_ip, ".");
	if (slist_count(ip_list) != 4)
		return 0;

	unsigned int n_ip = 0;
	int i = 0;

	for (slist_first(ip_list); !slist_end(ip_list); slist_next(ip_list))
	{
		char* data = slist_get(ip_list);
		unsigned int chunk = simple_strtoul(data, NULL, 10);
		n_ip |= (chunk << i++ * 8);
	}

	return n_ip;
}

UpdateLists* update_lists_new(void)
{
	UpdateLists* lists = (UpdateLists*)kmalloc(sizeof(UpdateLists), GFP_KERNEL);
	lists->ip_list_add = nlist_new();
	lists->ip_list_rem = nlist_new();
	lists->word_list_add = slist_new();
	lists->word_list_rem = slist_new();
	lists->clear_ips = FALSE;
	lists->clear_words = FALSE;
	return lists;
}

void update_lists_clear(UpdateLists* lists)
{
	if (lists->ip_list_add)
		nlist_clear(lists->ip_list_add);

	if (lists->ip_list_rem)
		nlist_clear(lists->ip_list_rem);

	if (lists->word_list_add)
		slist_clear(lists->word_list_add, TRUE);

	if (lists->word_list_rem)
		slist_clear(lists->word_list_rem, TRUE);

	lists->clear_ips = FALSE;
	lists->clear_words = FALSE;
}

void update_lists_delete(UpdateLists* lists)
{
	if (lists->ip_list_add)
		nlist_delete(lists->ip_list_add);

	if (lists->ip_list_rem)
		nlist_delete(lists->ip_list_rem);

	if (lists->word_list_add)
		slist_delete(lists->word_list_add, TRUE);

	if (lists->word_list_rem)
		slist_delete(lists->word_list_rem, TRUE);

	kfree(lists);
}

BOOL parse_command(const char* command,  UpdateLists* lists)
{
	update_lists_clear(lists);
	slist* cmd_list = split_string(command, " ");
	if (slist_empty(cmd_list))
		return FALSE;

	// Detect command
	slist_first(cmd_list);
	char* cmd = slist_get(cmd_list);
	BOOL ip_cmd = FALSE;
	BOOL word_cmd = FALSE;

	// IP command
	if (strcmp(cmd, IP_CMD) == 0)
		ip_cmd = TRUE;
	// WORD command
	else if (strcmp(cmd, WORD_CMD) == 0)
		word_cmd = TRUE;
	// Unknown
	else
	{
		pr_warn("Unknown command!\n");
		slist_delete(cmd_list, TRUE);
		return FALSE;
	}

	// Detect subcommand
	slist_next(cmd_list);
	char* sub_cmd = slist_get(cmd_list);
	CommandOp op = OpNone;

	// Add
	if (strcmp(sub_cmd, ADD_CMD) == 0)
		op = OpAdd;
	// Remove
	else if (strcmp(sub_cmd, REMOVE_CMD) == 0)
		op = OpRemove;
	// Clear
	else if (strcmp(sub_cmd, CLEAR_CMD) == 0)
		op = OpClear;
	// Unknown
	else
	{
		pr_warn("Unknown subcommand!\n");
		slist_delete(cmd_list, TRUE);
		return FALSE;
	}

	if (op == OpClear)
	{
		if (ip_cmd)
			lists->clear_ips = TRUE;
		if (word_cmd)
			lists->clear_words = TRUE;
	}
	else if (slist_next(cmd_list))
	{
		do
		{
			char* data = slist_get(cmd_list);
			if (ip_cmd && op == OpAdd)
				nlist_push(lists->ip_list_add, ipstr_to_uint(data));
			else if (ip_cmd && op == OpRemove)
				nlist_push(lists->ip_list_rem, ipstr_to_uint(data));
			else if (word_cmd && op == OpAdd)
				slist_push_copy(lists->word_list_add, data);
			else if (word_cmd && op == OpRemove)
				slist_push_copy(lists->word_list_rem, data);
		}
		while (slist_next(cmd_list));
	}

	slist_delete(cmd_list, TRUE);
	return TRUE;
}
