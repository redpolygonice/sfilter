#ifndef DATA_H
#define DATA_H

#include "types.h"
#include "nlist.h"
#include "slist.h"

BOOL data_load(const char* file_name, nlist* ip_list, slist* word_list);

#endif // DATA_H
