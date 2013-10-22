#ifndef __STRING_UTILS_H
#define __STRING_UTILS_H

#include "vector.h"

#define STRSPLIT_MAX_ELEM_LEN 1024

typedef void (*strsplit_cb)(const char*, void*);

void string_split_cb (strsplit_cb callback, void* cb_data, const char* string, const char* delimiter);
void string_split (vector_t* vector, const char* string, const char* delimiter);
int strpos (const char* string, const char* substring);
const char* string_chomp (const char* string, const char* substring);

#endif
