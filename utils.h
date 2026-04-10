#ifndef UTILS_H
#define UTILS_H

void clear_screen();
void clear_buffer();
void setup_files();
int compare(const char *str1, const char *str2);
int read_valid_name(char *buffer, int max_len, const char *prompt);

#endif
