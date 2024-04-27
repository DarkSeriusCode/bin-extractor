#ifndef __UTILS_H
#define __UTILS_H

#include <sys/stat.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define COLOR_GREEN  "\033[32m"
#define COLOR_RED    "\033[31m"

#define error(text, ...) fprintf(stderr, text "\n", ##__VA_ARGS__); exit(1)
#define log(color, title, text, ...) printf("[\033[1m" color "%s\033[0m]: " text, title, ##__VA_ARGS__ )

int mkdir_recursively(char *path);
bool contains(const char **heap, const char *needle);

#endif
