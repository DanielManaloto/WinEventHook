#include "file_handler.h"
#include <windows.h>
#include <stdio.h>
#include <process.h>

char** list_files(const char* path, int* count)
{
    if (!path || !count)
        return NULL;

    *count = 0;

    WIN32_FIND_DATA fdFile;
    HANDLE hFind = INVALID_HANDLE_VALUE;

    char searchPath[MAX_PATH];

    int written = snprintf(searchPath, sizeof(searchPath), "%s\\*", path);
    if (written < 0 || written >= (int)sizeof(searchPath))
        return NULL;

    hFind = FindFirstFile(searchPath, &fdFile);
    if (hFind == INVALID_HANDLE_VALUE)
        return NULL;

    int capacity = 16;
    char** files = malloc(capacity * sizeof(char*));

    if (!files) {
        FindClose(hFind);
        return NULL;
    }

    do {
        if (strcmp(fdFile.cFileName, ".") == 0 ||
            strcmp(fdFile.cFileName, "..") == 0)
            continue;

        if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            continue;

        char fullPath[MAX_PATH];

        written = snprintf(
            fullPath,
            sizeof(fullPath),
            "%s\\%s",
            path,
            fdFile.cFileName
        );

        if (written < 0 || written >= (int)sizeof(fullPath))
            continue;

        if (*count >= capacity) {
            capacity *= 2;

            char** temp = realloc(files, capacity * sizeof(char*));
            if (!temp) {
                for (int i = 0; i < *count; i++)
                    free(files[i]);
                free(files);
                FindClose(hFind);
                return NULL;
            }

            files = temp;
        }

        size_t len = strlen(fullPath);

        files[*count] = malloc(len + 1);
        if (!files[*count]) {
            for (int i = 0; i < *count; i++)
                free(files[i]);
            free(files);
            FindClose(hFind);
            return NULL;
        }

        memcpy(files[*count], fullPath, len + 1);
        (*count)++;

    } while (FindNextFile(hFind, &fdFile));

    FindClose(hFind);

    return files;
}


void free_file_list(char** files, int count)
{
    if (!files)
        return;

    for (int i = 0; i < count; i++)
        free(files[i]);

    free(files);
}


