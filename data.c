#include "data.h"
#include "command.h"

#include <linux/string.h>
#include <linux/fs.h>

enum
{
	NONE,
	IP,
	WORD
};

BOOL data_load(const char* file_name, nset* ip_set, slist* word_list)
{
	struct file *file = filp_open(file_name, O_RDONLY, 0);
	if (IS_ERR(file))
	{
		pr_err("Can't open data file!\n");
		return FALSE;
	}

	char buffer[1] = {0};
	char chunk[MAX_BUFFER] = {0};
	ssize_t bytes = 0;
	ssize_t count = 0;
	int type = NONE;
	loff_t pos = 0;
	do
	{
		bytes = kernel_read(file, buffer, 1, &pos);
		chunk[count] = buffer[0];

		if (buffer[0] == ';')
		{
			chunk[count] = '\0';

			if (strcmp(chunk, "ip") == 0)
			{
				type = IP;
			}
			else if (strcmp(chunk, "word") == 0)
			{
				type = WORD;
			}
			else
			{
				type = NONE;
			}

			count = 0;
			memset(chunk, 0, MAX_BUFFER);
		}
		else if (buffer[0] == '\n')
		{
			chunk[count] = '\0';

			if (chunk[0] == 0)
				break;

			if (type == IP)
			{
				unsigned int ip = ipstr_to_uint(chunk);
				nset_insert(ip_set, ip);
			}
			else if (type == WORD)
			{
				slist_push_copy(word_list, chunk);
			}

			count = 0;
			memset(chunk, 0, MAX_BUFFER);
		}
		else
		{
			count++;
		}

	} while (bytes > 0);

	filp_close(file, NULL);
	return TRUE;
}
