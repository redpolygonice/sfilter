#include "filter.h"
#include "nlist.h"
#include "slist.h"
#include "command.h"

#include <linux/slab.h>
#include <linux/string.h>

static nlist* ip_list = NULL;
static slist* word_list = NULL;
static UpdateLists* update_lists = NULL;

void filter_init(void)
{
	ip_list = nlist_new();
	word_list = slist_new();
	update_lists = update_lists_new();
}

void filter_clear(void)
{
	update_lists_delete(update_lists);
	nlist_delete(ip_list);
	slist_delete(word_list, TRUE);
}

void filter_update_lists(const char*command)
{
	update_lists_clear(update_lists);
	if (!parse_command(command, update_lists))
		return;

	// Add IP
	if (!nlist_empty(update_lists->ip_list_add))
		nlist_append_unique(ip_list, update_lists->ip_list_add);

	// Remove IP
	if (!nlist_empty(update_lists->ip_list_rem))
		nlist_remove_list(ip_list, update_lists->ip_list_rem);

	// Add word
	if (!slist_empty(update_lists->word_list_add))
		slist_append_unique(word_list, update_lists->word_list_add);

	// Remove word
	if (!slist_empty(update_lists->word_list_rem))
		slist_remove_list(word_list, update_lists->word_list_rem);

	// Clear IPs
	if (update_lists->clear_ips)
		nlist_clear(ip_list);

	// Clear words
	if (update_lists->clear_words)
		slist_clear(word_list, TRUE);

	if (!nlist_empty(ip_list))
	{
		pr_info("Netfilter: IP list\n");
		nlist_print_ip(ip_list);
	}

	if (!slist_empty(word_list))
	{
		pr_info("Netfilter: Word list\n");
		slist_print(word_list);
	}
}

BOOL filter_process_ip(unsigned int src_ip, unsigned int dest_ip)
{
	if (nlist_find(ip_list, src_ip))
		return FALSE;

	return TRUE;
}

BOOL filter_process_data(const char* data, char** word)
{
	if (slist_find_part(word_list, data))
	{
		char* found_word = slist_get(word_list);
		if (word != NULL)
			*word = kstrdup(found_word, GFP_KERNEL);
		return FALSE;
	}

	return TRUE;
}
