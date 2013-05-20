#ifndef __FILE_UTILS_H
#define __FILE_UTILS_H

#define FILE_READ_CHUNK_SIZE 4096
#define FILE_MAX_LINE_LEN 1024

void read_file (const char* filename, void (*chunk_handler)(const char*, int, int));
void file_get_lines (const char* filename, void (*line_handler)(const char*));

#endif
