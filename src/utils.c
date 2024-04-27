#include "utils.h"

int mkdir_recursively(char *path) {
    char *sep = strrchr(path, '/');
    if(sep != NULL) {
        *sep = 0;
        mkdir_recursively(path);
        *sep = '/';
    }
    if(mkdir(path, 0777) && errno != EEXIST)
        return 1;
    return 0;
}

bool contains(const char **heap, const char *needle) {
    for (size_t i = 0; heap[i] != NULL; i++) {
        if (strcmp(heap[i], needle) == 0) {
            return true;
        }
    }
    return false;
}
