#ifndef __FILE_UTILS_H
#define __FILE_UTILS_H

#include "string_utils.h"

#define FILE_READ_CHUNK_SIZE 4096
#define FILE_MAX_LINE_LEN 1024

typedef void (*read_file_cb)(const char*, int, int, void*);

void read_file (const char* filename, read_file_cb chunk_handler, void* cb_data);
void file_get_lines (const char* filename, strsplit_cb line_handler);

#endif
