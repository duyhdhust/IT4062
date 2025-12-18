#ifndef UTILS_H
#define UTILS_H

#include "common.h"

// Logging
void write_log(char *msg);

// Network utilities
int recv_line(int sock, char *buff, int max_len);

#endif
