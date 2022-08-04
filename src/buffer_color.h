#ifndef BUFFER_COLOR_H
#define BUFFER_COLOR_H

#include "./buffer.h"

#define INTERVALS_SIZE 64
#define LINE_SIZE 256

typedef struct{
  size_t start, end;
  int color;
}Interval;

typedef struct{
  size_t line;
  Interval *intervals;
  size_t intervals_count;
  size_t intervals_cap;
}Line;

typedef struct{
  Line *lines;
  size_t lines_count;
  size_t lines_cap;

  char **keywords;
  size_t *keywords_len;
  int *keywords_colors;
  size_t keywords_count;
  
  char *sentence_open;
  char *sentence_closed;
  int *sentence_colors;
  size_t sentence_count;
}Text;

void text_init(Text *text);
void text_quit(Text *text);
void text_dump(const Text *text);
void text_read_file(Text *text, const char* file_path);

bool line_eol(const Text *text, char *buffer, size_t len, int *color, size_t *end);
bool isText(char *d);

void text_update(Text *text, const Buffer *buffer, const Cursor *cursor, size_t y, size_t height);

#endif //BUFFER_COLOR_H
