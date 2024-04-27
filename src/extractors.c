#include "extractors.h"
#include "utils.h"

int filecontent_init(FileContent *fc, const char *path) {
    struct stat fstat;
    if (stat(path, &fstat) != 0) {
        return 1; // Means there's no such file
    }
    fc->fname = path;
    fc->len = fstat.st_size;

    int fd = open(path, O_RDONLY);
    fc->data = mmap(0, fstat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    return 0;
}

void filecontent_delete(FileContent *fc) {
    munmap(fc->data, fc->len);
}

// ------------------------------------------------------------------------------------------------

Finder get_finder_by_ext(const char *ext) {
    FinderFunc fn_ptr = NULL;

    if (contains(OGG_EXTS, ext)) {
        fn_ptr = &find_ogg;
    }
    else if (contains(BMP_EXTS, ext)) {
        fn_ptr = &find_bmp;
    }
    else if (contains(PNG_EXTS, ext)) {
        fn_ptr = &find_png;
    }
    else if (contains(JPEG_EXTS, ext)) {
        fn_ptr = &find_jpeg;
    }
    return (Finder){ fn_ptr, ext };
}

long extract_file(FileContent fc, Finder finder, long offset, const char *dest_dir) {
    byte *begin = NULL;
    byte *end = NULL;
    char extracted_fname[strlen(fc.fname) + strlen(dest_dir) + strlen(finder.ext) + 4];

    int res = finder.finder_fn(fc.data, fc.len, offset, &begin, &end);
    if (!res) {
        return -1;
    }
    sprintf(extracted_fname, "%s/%s-%lx.%s", dest_dir, fc.fname, offset, finder.ext);
    FILE *fp = fopen(extracted_fname, "w");
    if (!fp) {
        log(COLOR_RED, finder.ext, "Cannot create file %s\n", extracted_fname);
        return -1;
    }
    size_t file_len = end - begin;
    fwrite(begin, sizeof(byte), file_len, fp);
    fclose(fp);
    return offset + file_len;
}

// ------------------------------------------------------------------------------------------------

static size_t ogg_page_len(const byte *page) {
    byte nsegs = page[26];
    const byte *segs = page + 27;
    int length = 27 + nsegs;

    for (int i = 0; i < nsegs; i++) {
        length += segs[i];
    }
    return length;
}

int find_ogg(byte *data, size_t data_len, size_t offset, byte **begin, byte **end) {
    const int OGG_HEADER = 0x5367674f;
    *begin = *end = NULL;

    for (size_t i = offset; i < data_len; i++) {
        byte *ptr = data + i;
        if (*(int*)ptr == OGG_HEADER) {
            // If page is BOS
            if (ptr[5] & 2) {
                *begin = ptr;
            }
            // If page in EOS and begin has already found
            if (ptr[5] & 4 && *begin) {
                *end = ptr + ogg_page_len(ptr);
                return 1;
            }
        }
    }
    return 0;
}

int find_bmp(byte *data, size_t data_len, size_t offset, byte **begin, byte **end) {
    const short BMP_HEADER = 0x4d42;
    *begin = *end = NULL;

    for (size_t i = offset; i < data_len; i++) {
        byte *ptr = data + i;
        if (*(short*)ptr == BMP_HEADER) {
            unsigned int bmp_size = *((unsigned int *)(ptr + 2));
            *begin = ptr;
            *end = ptr + bmp_size;
            return 1;
        }
    }

    return 0;
}

int find_png(byte *data, size_t data_len, size_t offset, byte **begin, byte **end) {
    const unsigned long PNG_HEADER = 0x0a1a0a0d474e5089;
    const unsigned int IEND_SIGN = 0x444e4549;
    *begin = *end = NULL;

    for (size_t i = offset; i < data_len; i++) {
        byte *ptr = data + i;
        if (*(unsigned long*)ptr == PNG_HEADER && !*begin) {
            *begin = ptr;
        }
        if (*(unsigned int *)ptr == IEND_SIGN && *begin) {
            *end = ptr;
            return 1;
        }
    }

    return 0;
}

int find_jpeg(byte *data, size_t data_len, size_t offset, byte **begin, byte **end) {
    const unsigned int JPEG_HEADER = 0xe0ffd8ff;
    const unsigned short JPEG_END_SIGN = 0xd9ff;
    *begin = *end = NULL;

    for (size_t i = offset; i < data_len; i++) {
        byte *ptr = data + i;
        if (*(unsigned int *)ptr == JPEG_HEADER && !*begin) {
            *begin = ptr;
        }
        if (*(unsigned short *)ptr == JPEG_END_SIGN && *begin) {
            *end = ptr;
            return 1;
        }
    }

    return 0;
}
