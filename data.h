#ifndef DATA_H
#define DATA_H

#include "types.h"
#include "nset.h"
#include "slist.h"

BOOL data_load(const char* file_name, nset* ip_set, slist* word_list);

#endif // DATA_H
