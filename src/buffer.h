#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include "./js.h"
#include "./vec.h"

#define BUFFER_SIZE 1024
#define LINES_SIZE 128

typedef struct{
  char *buffer;
  size_t buffer_size;
  size_t buffer_cap;

  size_t *lines;
  size_t *positions;
    
  size_t lines_size;
  size_t lines_cap;
}Buffer;

void buffer_init(Buffer *buffer);
void buffer_quit(Buffer *buffer);

typedef struct{
  size_t x, y, i, j;
  size_t pos;
}Cursor;

void cursor_log(const Cursor *cursor);

void cursor_click(Cursor *cursor,
    const Buffer *buffer,
    size_t _x, size_t _y,
    size_t char_width, size_t char_height,
    bool sec
    );

void cursor_reset(Cursor *cursor);
size_t cursor_dist(Cursor *cursor, const Buffer *buffer, size_t *start, size_t *end);
void cursor_outer(Cursor *cursor, const Buffer *buffer);

void cursor_right(Cursor *cursor, const Buffer *buffer, size_t size, bool shift);
void cursor_left(Cursor *cursor, const Buffer *buffer, size_t size, bool shift);
void cursor_up(Cursor *cursor, const Buffer *buffer, size_t size, bool shift);
void cursor_down(Cursor *cursor, const Buffer *buffer, size_t size, bool shift);

void cursor_beg_line(Cursor *cursor, const Buffer *buffer, bool shift);
void cursor_end_line(Cursor *cursor, const Buffer *buffer, bool shift);
void cursor_word_right(Cursor *cursor, const Buffer *buffer);
void cursor_word_left(Cursor *cursor, const Buffer *buffer);

void buffer_process_lines(Buffer *buffer);
void buffer_lines_log(const Buffer *buffer);
void buffer_insert_size(Buffer *buffer, Cursor *cursor, const char* text, size_t size);
void buffer_insert(Buffer *buffer, Cursor *cursor, const char* text);
void buffer_delete(Buffer *buffer, Cursor *cursor, size_t size);

void buffer_load_file(Buffer *buffer, const char* file_path);
void buffer_save_file(const Buffer *buffer, const char* file_path);

#endif //BUFFER_H
