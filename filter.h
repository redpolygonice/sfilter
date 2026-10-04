#ifndef FILTER_H
#define FILTER_H

#include "types.h"

void filter_init(void);
void filter_clear(void);
void filter_update_lists(const char* command);
unsigned int filter_process_ip(unsigned int src_ip, unsigned int dest_ip);
BOOL filter_process_data(const char* data, char** word);

#endif // FILTER_H
