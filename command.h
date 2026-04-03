#ifndef COMMAND_H
#define COMMAND_H

#include "slist.h"
#include "nlist.h"

#define IP_CMD "ip"
#define WORD_CMD "word"
#define ADD_CMD "add"
#define REMOVE_CMD "rem"
#define CLEAR_CMD "clear"

typedef enum CommandOp
{
	OpNone,
	OpAdd,
	OpRemove,
	OpClear
} CommandOp;

typedef struct UpdateLists
{
	nlist* ip_list_add;
	nlist* ip_list_rem;
	slist* word_list_add;
	slist* word_list_rem;
	BOOL clear_ips;
	BOOL clear_words;
} UpdateLists;

UpdateLists* update_lists_new(void);
void update_lists_clear(UpdateLists* lists);
void update_lists_delete(UpdateLists* lists);

BOOL parse_command(const char* command, UpdateLists* lists);

#endif // COMMAND_H
