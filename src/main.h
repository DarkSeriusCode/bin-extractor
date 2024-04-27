#ifndef __MAIN_H
#define __MAIN_H

#include "extractors.h"

void print_help_message(void);
void print_supported_formats(void);
bool argv_has_all(int argc, char *argv[]);
void extract_all_files(FileContent fc, char *format);

#endif
