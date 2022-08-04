#ifndef JS_H
#define JS_H

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <limits.h>
#include <ctype.h>

typedef struct {
	size_t count;
	const char* data;
} string;

#define String(cstr_lit) string_from_parts(cstr_lit, sizeof(cstr_lit) - 1)

string string_from_parts(const char* data, size_t count);
string string_from_cstr(const char* data);
string string_trim_left(string s);
string string_trim_right(string s);
string string_trim(string s);
string string_chop_by_delim(string *s, char delim);
void string_print(string s);
string string_chop_left(string *s, size_t n);
int string_chop_int(string *s, int *n);
int string_chop_int64_t(string *s, int64_t *n);

char* string_to_cstr(string *s);
int string_to_int(const string s, int *n);

char* slurp_file(const char* file_path, size_t *size);

int char_to_int(char s, int *target);

#endif //JS_H

