#pragma once
#ifdef __cplusplus
extern "C" {
#endif

char** list_files(const char* path, int* count);
void free_file_list(char** files, int count);


#ifdef __cplusplus
}
#endif