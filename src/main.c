#include <stdio.h>
#include <getopt.h>
#include "config.h"
#include "extractors.h"
#include "utils.h"
#include "main.h"

// TODO: mp3, mp4, wav, webp, m4a, gif

struct option long_options[] = {
    { "list", no_argument, 0, 'l' },
    { "help", no_argument, 0, 'h' },
    { 0, 0, 0, 0 },
};

int main(int argc, char *argv[]) {
    int opt = getopt_long(argc, argv, "lh", long_options, NULL);
    switch (opt) {
        case 'l':
            print_supported_formats();
            return 0;
        case 'h':
            print_help_message();
            return 0;
    }
    if (argc < 3) {
        error("Usage: bin-extractor [-h] [-l] <file formats> <file>");
    }

    short format_count;
    char **input_formats;
    bool has_all = argv_has_all(argc, argv);
    if (has_all) {
        format_count = SUPPORTED_FORMATS_COUNT;
        input_formats = (char **)SUPPORTED_FORMATS;
        printf("\"all\" is specified so trying to extract all supported formats\n");
    } else {
        format_count = argc - 2;
        input_formats = &argv[1];
    }
    const char *input_file = argv[argc - 1];

    FileContent file_content;
    if (filecontent_init(&file_content, input_file) == 1) {
        error("There's no such file \"%s\"", input_file);
    }

    for (int i = 0; i < format_count; i++) {
        char *format = input_formats[i];
        extract_all_files(file_content, format);
    }

    filecontent_delete(&file_content);
    return 0;
}

void print_help_message() {
    printf("bin-extractor - A program that extracts images and audio from binaries.\n");
    printf("Usage: bin-extractor [-h] [-l] <file formats> <input binary file>\n");
    printf("\nOptions:\n  -l\t- Prints all supported file formats to extract\n");
    printf("  -h\t- Print this message.\n");
}

void print_supported_formats(void) {
    printf("Supported formats: ");
    for (int i = 0; i < SUPPORTED_FORMATS_COUNT; i++) {
        printf("%s ", SUPPORTED_FORMATS[i]);
    }
    printf("\nYou can use \"all\" as a format. ");
    printf("In this case all supported formats will be extracted\n");
}

bool argv_has_all(int argc, char *argv[]) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], "all") == 0) {
            return true;
        }
    }
    return false;
}

void extract_all_files(FileContent fc, char *format) {
    Finder finder = get_finder_by_ext(format);
    if (!finder.finder_fn) {
        log(COLOR_RED, format, "This format is not supported!\n");
        return;
    }

    char path_buf[strlen(OUTPUT_DIR) + strlen(fc.fname) + strlen(format) + 2];
    sprintf(path_buf, "%s/%s/%s", OUTPUT_DIR, fc.fname, format);
    if (mkdir_recursively(path_buf) == 1) {
        error("Cannot create %s", path_buf);
    }

    log(COLOR_GREEN, format, "Extracting files from %s\n", fc.fname);
    long offset = 0;
    size_t extracted_file_count = 0;

    while ( (offset = extract_file(fc, finder, offset, path_buf)) >= 0 ) {
        log(COLOR_GREEN, format, "Extracted %zd file(s)", ++extracted_file_count);
        fflush(stdout);
        printf("\r");
    }

    if (extracted_file_count == 0) {
        log(COLOR_RED, format, "File %s has no %s files\n", fc.fname, format);
        rmdir(path_buf);
    } else {
        log(COLOR_GREEN, format, "Successfuly extracted %zd files\n", extracted_file_count);
    }
    printf("\n");
}
