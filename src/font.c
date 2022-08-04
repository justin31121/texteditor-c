#include "./font.h"

void font_init(Font *f, SDL_Renderer *rend, const char* path, size_t font_size) {
    const char alphabet[] = 
     " !\"#$%&\'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";
  
    TTF_Font *font = stcp(TTF_OpenFont(path, font_size));

    SDL_Color white = {decode(0xffffffff)};

    SDL_Surface *surface =
        scp(TTF_RenderUTF8_Blended(font, alphabet, white));

    SDL_Texture *texture =
        scp(SDL_CreateTextureFromSurface(rend, surface));

    int w, h;
    TTF_SizeText(font, alphabet, &w, &h);
    size_t len = strlen(alphabet);

    f->texture = texture;
    f->width = (int) (w/len);
    f->height = h;

    for(size_t i=0;i<ASCII_LEN;i++) {
        f->glyphs[i] = (SDL_Rect) {
            i*f->width,
            0,
            f->width,
            f->height
        };
    }

    SDL_FreeSurface(surface);
}

void font_close(Font *font) {
    SDL_DestroyTexture(font->texture);
}

float font_render_char(SDL_Renderer *rend, const Font *font, Vec pos, char c, float scale,
    Uint32 color) {

  if(c<ASCII_LOW) {
    return 0.0;
  }
    
  int i = c - ASCII_LOW;
    
  SDL_Rect dst = {
    pos.x, pos.y,
    font->width * scale,
    font->height * scale};

  SDL_SetTextureColorMod(font->texture, decode_wo_a(color));
  SDL_SetTextureAlphaMod(font->texture, decode_a(color));

  SDL_RenderCopy(rend, font->texture, &font->glyphs[i], &dst);
    
  return dst.w;
}

void font_render_text(SDL_Renderer *rend, const Font *font, Vec pos, const char* cs, float scale, Uint32 color) {
  size_t len = strlen(cs);

  font_render_text_sized(rend, font, pos, cs, len, scale, color);
}

void font_render_text_sized(SDL_Renderer *rend, const Font *font, Vec pos, const char* cs, size_t size, float scale, Uint32 color) {
  for(size_t i=0;i<size;i++) {
    float off = font_render_char(rend, font, pos, cs[i], scale, color);
    pos.x += off;
  }
}

