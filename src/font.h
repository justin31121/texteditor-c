#ifndef FONT_H
#define FONT_H

#define ASCII_LOW (int) 32
#define ASCII_HIGH (int) 127
#define ASCII_LEN ASCII_HIGH - ASCII_LOW

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "./sdl_c.h"
#include "./vec.h"

#define decode(c) c >> (8 * 3) & 0xff, c >> (8 * 2) & 0xff, c >> (8 * 1) & 0xff, c >> (8 * 0) & 0xff
#define decode_wo_a(c) c >> (8 * 3) & 0xff, c >> (8 * 2) & 0xff, c >> (8 * 1) & 0xff
#define decode_a(c) c >> (8 * 0) & 0xff

typedef struct{
    SDL_Texture *texture;
    SDL_Rect glyphs[ASCII_LEN];
    int width;
    int height;
}Font;

void font_init(Font *f, SDL_Renderer *rend, const char* path, size_t font_size);
void font_close(Font *font);

float font_render_char(SDL_Renderer *rend, const Font *font, Vec pos, char c, float scale, Uint32 color);
void font_render_text(SDL_Renderer *rend, const Font *font, Vec pos, const char* cs, float scale, Uint32 color);
void font_render_text_sized(SDL_Renderer *rend, const Font *font, Vec pos, const char* cs, size_t size, float scale, Uint32 color);

#endif //FONT_H
