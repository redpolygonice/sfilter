#include "filter.h"
#include "container.h"
#include "command.h"
#include "data.h"

#include <linux/slab.h>
#include <linux/string.h>
#include <linux/mutex.h>

static DEFINE_MUTEX(ip_mutex);
static DEFINE_MUTEX(word_mutex);

static nset* ip_set = NULL;
static slist* word_list = NULL;
static UpdateLists* update_lists = NULL;

void filter_init(void)
{
	ip_set = nset_new();
	word_list = slist_new();
	update_lists = update_lists_new();

	if (!data_load(DATA_FILE_NAME, ip_set, word_list))
		pr_info("Netfilter: Init data lists empty!\n");
	else
	{
		pr_info("Netfilter: Init data lists:\n");

		if (!nset_empty(ip_set))
			nset_print_ip(ip_set);

		if (!slist_empty(word_list))
			slist_print(word_list);
	}
}

void filter_clear(void)
{
	update_lists_delete(update_lists);
	nset_delete(ip_set);
	slist_delete(word_list, TRUE);
}

void filter_update_lists(const char*command)
{
	update_lists_clear(update_lists);
	if (!parse_command(command, update_lists))
		return;

	pr_info("Netfilter: Update lists:\n");
	mutex_lock_interruptible(&ip_mutex);

	// Add IP
	if (!nlist_empty(update_lists->ip_list_add))
		nset_append_list(ip_set, update_lists->ip_list_add);

	// Remove IP
	if (!nlist_empty(update_lists->ip_list_rem))
		nset_remove_list(ip_set, update_lists->ip_list_rem);

	// Clear IPs
	if (update_lists->clear_ips)
		nset_clear(ip_set);

	if (!nset_empty(ip_set))
	{
		pr_info("Netfilter: IP list\n");
		nset_print_ip(ip_set);
	}
	else
		pr_info("Netfilter: IP list empty\n");

	mutex_unlock(&ip_mutex);
	mutex_lock_interruptible(&word_mutex);

	// Add word
	if (!slist_empty(update_lists->word_list_add))
		slist_append_unique(word_list, update_lists->word_list_add);

	// Remove word
	if (!slist_empty(update_lists->word_list_rem))
		slist_remove_list(word_list, update_lists->word_list_rem);

	// Clear words
	if (update_lists->clear_words)
		slist_clear(word_list, TRUE);

	if (!slist_empty(word_list))
	{
		pr_info("Netfilter: Word list\n");
		slist_print(word_list);
	}
	else
		pr_info("Netfilter: Word list empty\n");

	mutex_unlock(&word_mutex);
}

unsigned int filter_process_ip(unsigned int src_ip, unsigned int dest_ip)
{
	mutex_lock_interruptible(&ip_mutex);
	if (nset_find(ip_set, src_ip))
	{
		mutex_unlock(&ip_mutex);
		return src_ip;
	}

	if (nset_find(ip_set, dest_ip))
	{
		mutex_unlock(&ip_mutex);
		return dest_ip;
	}

	mutex_unlock(&ip_mutex);
	return 0;
}

BOOL filter_process_data(const char* data, char** word)
{
	mutex_lock_interruptible(&word_mutex);
	if (slist_find_part(word_list, data))
	{
		char* found_word = slist_get(word_list);
		if (word != NULL)
			*word = kstrdup(found_word, GFP_KERNEL);
		mutex_unlock(&word_mutex);
		return FALSE;
	}

	mutex_unlock(&word_mutex);
	return TRUE;
}
