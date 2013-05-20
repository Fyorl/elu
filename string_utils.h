#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include "vector.h"

#define STRSPLIT_MAX_ELEM_LEN 1024

void string_split_cb (void (*callback)(const char*), const char* string, char delimiter);
void string_split (vector_t* vector, const char* string, char delimiter);

#endif
