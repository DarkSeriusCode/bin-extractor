#ifndef __EXTRACTORS_H
#define __EXTRACTORS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include "config.h"
#include "utils.h"

#define SUPPORTED_FORMATS_COUNT 4
static const char *SUPPORTED_FORMATS[SUPPORTED_FORMATS_COUNT] = { "png", "ogg", "bmp", "jpeg" };

typedef unsigned char byte;

typedef struct {
    const char *fname;
    byte *data;
    size_t len;
} FileContent;

int filecontent_init(FileContent *fc, const char *path);
void filecontent_delete(FileContent *fc);

// ------------------------------------------------------------------------------------------------

typedef int(*FinderFunc)(byte *data, size_t data_len, size_t offset, byte **begin, byte **end);

typedef struct {
    FinderFunc finder_fn;
    const char *ext;
} Finder;

Finder get_finder_by_ext(const char *ext);

long extract_file(FileContent fc, Finder finder, long offset, const char *dest_dir);

// ------------------------------------------------------------------------------------------------

static const char *OGG_EXTS[] = { "ogg" };
int find_ogg(byte *data, size_t data_len, size_t offset, byte **begin, byte **end);

static const char *BMP_EXTS[] = { "bmp" };
int find_bmp(byte *data, size_t data_len, size_t offset, byte **begin, byte **end);

static const char *PNG_EXTS[] = { "png" };
int find_png(byte *data, size_t data_len, size_t offset, byte **begin, byte **end);

static const char *JPEG_EXTS[] = { "jpg", "jpeg" };
int find_jpeg(byte *data, size_t data_len, size_t offset, byte **begin, byte **end);

#endif
