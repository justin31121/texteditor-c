#include "./sdl_c.h"
#include "./font.h"

#include "./vec.h"
#include "./buffer.h"
#include "./buffer_color.h"

#define BG_COLOR 0x202020ff
#define FONT_COLOR 0xffffffff
#define FONT_DARKER_COLOR 0x616161ff
#define HG_COLOR 0x606060ff
#define YELLOW 0xf1d302ff
#define GREEN 0x00aa00ff
#define DEFINE_GRAY 0xbbbbbbff

#define WIDTH 800
#define HEIGHT 600

#define PAD_LEFT 25
#define PAD_TOP 5

#define FONT_PATH "./rsc/iosevka-regular.ttf" //"./rsc/consola.ttf"
#define FONT_SIZE 18

#define ENTER 10
#define TAB_WIDTH 4

size_t font_size = FONT_SIZE;

size_t height;
size_t width;
size_t x = 0;
size_t y = 0;

size_t off = 0;

bool running = true;

bool ctrl = false;
bool shift = false;
bool alt = false;
bool drag = false;

Uint32 colors[] = {FONT_COLOR, YELLOW, DEFINE_GRAY, GREEN};

void load_number_in_buffer(int n, char *buffer, int size) {  
  for(int i=size-2;i>=0;i--) {
    int m = n % 10;
    if(n>0) {
      buffer[i] = m +  '0';
    }
    else {
      buffer[i] = ' ';
    }
    n = n/10;
  }
}

bool qoute = false;


void render(SDL_Renderer *rend, Font *font, Buffer *buffer, const Cursor *cursor, const Text *text, bool c) {
  SDL_RenderClear(rend);
  float scale = 1.0;
  
  float offset = 3;
  size_t f = buffer->lines_size;
  while((f=f/10)>0) offset++;

  Vec origin = vec((x*font->width) - offset * font->width, y*font->height - (float) PAD_TOP);
  
  //DRAW HIGHLIHGTED TEXT
  Vec pos = vec(cursor->x, cursor->y);
  Vec old = vec(cursor->i, cursor->j);

  if(!vec_eq(pos, old)) {
    Vec min = vec_min(pos, old);
    Vec max = vec_max(pos, old);

    for(int i=min.y;i<=max.y;i++) {
      float x1;
      if(i==min.y) {
	x1 = min.x;
      }
      else {
	x1 = 0;
      }

      float x2 = width;
      if(i==max.y) {
	x2 = max.x - x1;
      }

      SDL_Rect temp = {x1*font->width - origin.x, i*font->height - origin.y, x2*font->width, font->height};
      SDL_SetRenderDrawColor(rend, decode(HG_COLOR));
      SDL_RenderFillRect(rend, &temp);
    }
  }

  //DRAW BUFFER
  off = ((int) offset) - 1;
  char number_buffer[off];
  number_buffer[off-1] = 0;
  size_t k = y+1;

  //DRAW TEXT
  for(size_t i=0;i<text->lines_count;i++) {
    Line line = text->lines[i];
    for(size_t j=0;j<line.intervals_count;j++) {
      Interval inter = line.intervals[j];
      if(inter.end==0) continue;
      font_render_text_sized(rend, font, vec(inter.start*font->width - origin.x, (line.line)*font->height*scale - origin.y),
			     buffer->buffer + buffer->positions[line.line] + inter.start, inter.end,
			     scale, colors[inter.color]);
    }

    //DRAW LINE
    load_number_in_buffer(k, number_buffer, off);
    Uint32 color = FONT_DARKER_COLOR;
    if(k==cursor->y+1) {
      color = YELLOW;
    }
    font_render_text_sized(rend, font, vec(0 - origin.x -  off*font->width, (i+y)*font->height*scale - origin.y),
			   number_buffer, offset-1,
			   scale, color);
    k++;
  }
  
  //DRAW CURSOR
  if(c) {
    SDL_Rect rect = {
      cursor->x*font->width*scale - origin.x,
      cursor->y*font->height*scale - origin.y,
      font->width*scale, font->height*scale};
    SDL_SetRenderDrawColor(rend, decode(YELLOW));
    SDL_RenderFillRect(rend, &rect);
    //DRAW CHAR BEHIND CURSOR
    if(cursor->pos<buffer->buffer_size) {
      font_render_char(rend, font,
		       vec(cursor->x*font->width*scale - origin.x,
			   cursor->y*font->height*scale - origin.y),
		       buffer->buffer[cursor->pos], scale, BG_COLOR);   
    }
  }

  //RENDER BACKGROUND
  SDL_SetRenderDrawColor(rend, decode(BG_COLOR));
  SDL_RenderPresent(rend);
}

//void render(SDL_Renderer *rend, Font *font, Buffer *buffer, const Cursor *cursor) {
typedef struct{
  SDL_Window *wind;
  SDL_Renderer *rend;
  Font *font;
  Buffer *buffer;
  Cursor *cursor;
  Text *text;
  bool *show;
}PassingStruct;

static int resizingEventWatcher(void* data, SDL_Event* event) {
  if (event->type == SDL_WINDOWEVENT &&
      event->window.event == SDL_WINDOWEVENT_RESIZED) {
    SDL_Window* win = SDL_GetWindowFromID(event->window.windowID);
    PassingStruct* p = (PassingStruct*)data;
    if (win == p->wind) {
      render(p->rend, p->font, p->buffer, p->cursor, p->text, true);

      int w, h;
      SDL_GetWindowSize(p->wind, &w, &h);
      
      width = w / p->font->width;
      height = h / p->font->height;
    }
  }
  return 0;
}

void window_fit(const Buffer *buffer, const Cursor *cursor) {
  (void) buffer;
    
  while(cursor->y>=y+height) y++;
  while(cursor->y<y) y--;

  while(cursor->x>=x+width-2) x++;
  while(cursor->x<x) x--;
}

void key_ctrl(size_t keycode, bool on) {
  switch(keycode) {
  case SDLK_LCTRL:
    ctrl = on;
    break;
  case SDLK_RCTRL:
    ctrl = on;
    break;
  case SDLK_LSHIFT:
    shift = on;
    break;
  case SDLK_RSHIFT:
    shift = on;
    break;
  case SDLK_LALT:
    alt = on;
    break;
  case SDLK_RALT:
    alt = on;
    break;
  default:
    break;
  }
}

void key_down(Buffer *buffer, Cursor *cursor, Font *font, SDL_Renderer *rend, size_t keycode, const char* file_path) {
  //printf("Pressed %lld\n", keycode);
  size_t size, start, end;
  char* content;
  if(ctrl) {
    switch(keycode) {
    case'-':
      font_close(font);
      if(font_size<2) break;;
      font_size -= 2;
      font_init(font, rend, FONT_PATH, font_size);
      height = HEIGHT / font->height;
      width = WIDTH / font->width;      
      break;
    case'+':
      font_close(font);
      font_size += 2;
      font_init(font, rend, FONT_PATH, font_size);
      height = HEIGHT / font->height;
      width = WIDTH / font->width;
      break;
    case 'g':
      cursor_reset(cursor);
      break;
    case 'y':
      content = SDL_GetClipboardText();
      buffer_insert(buffer, cursor, content);
      buffer_process_lines(buffer);
      SDL_free(content);
      break;
    case 'w':
      size = cursor_dist(cursor, buffer, &start, &end);
      if(size!=0) {
	//COPY TO CLIPBOARD
	char copy_content[size+1];
	memcpy(copy_content, buffer->buffer + start, size);
	copy_content[size] = 0;
 
	if(SDL_SetClipboardText(copy_content)!=0) {
	  printf("Cant copy text\n");   
	}
	cursor_outer(cursor, buffer);
	buffer_delete(buffer, cursor, size);
	buffer_process_lines(buffer);
      }
      break;
    case 'k':
      size = buffer->lines[cursor->y];
      cursor_right(cursor, buffer, size, false);
      buffer_delete(buffer, cursor, size);
      buffer_process_lines(buffer);
      break;
    case 's':
      if(file_path!=NULL) buffer_save_file(buffer, file_path);
      break;
    case 'e':
      cursor_end_line(cursor, buffer, shift);
      break;
    case 'a':
      cursor_beg_line(cursor, buffer, shift);
      break;
    case 'd':
      size = cursor_dist(cursor, buffer, NULL, NULL);
      if(size==0) {
	cursor_right(cursor, buffer, 1, false);
	buffer_delete(buffer, cursor, 1);
      }
      else {
	cursor_outer(cursor, buffer);
	buffer_delete(buffer, cursor, size);
      }
      buffer_process_lines(buffer);
      break;      
    case 'f':
      cursor_right(cursor, buffer, 1, shift);
      break;
    case 'b':
      cursor_left(cursor, buffer, 1, shift);
      break;
    case 'p':
      cursor_up(cursor, buffer, 1, shift);
      break;
    case 'n':
      cursor_down(cursor, buffer, 1, shift);
      break;
    default:
      break;
    }
  }

  if(alt) {
    switch(keycode) {
    case 'w':
      size = cursor_dist(cursor, buffer, &start, &end);
      if(size!=0) {
	//COPY TO CLIPBOARD
	char copy_content[size+1];
	memcpy(copy_content, buffer->buffer + start, end);
	copy_content[size] = 0;
 
	if(SDL_SetClipboardText(copy_content)!=0) {
	  printf("Cant copy text\n");   
	}
      }
      break;
    default:
      break;
    }
  }

  switch(keycode) {
  case SDLK_BACKSPACE:
    size = cursor_dist(cursor, buffer, NULL, NULL);
    if(size==0) {
      buffer_delete(buffer, cursor, 1);
    }
    else {
      cursor_outer(cursor, buffer);
      buffer_delete(buffer, cursor, size);
    }
    buffer_process_lines(buffer);
    break;
  case SDLK_ESCAPE:
    running = false;
    break;
  case SDLK_UP:
    cursor_up(cursor, buffer, 1, shift);
    break;
  case SDLK_DOWN:
    cursor_down(cursor, buffer, 1, shift);
    break;
  case SDLK_RIGHT:
    cursor_right(cursor, buffer, 1, shift);
    break;
  case SDLK_LEFT:
    cursor_left(cursor, buffer, 1, shift);
    break;
  case SDLK_RETURN:
    size = cursor_dist(cursor, buffer, NULL, NULL);
    if(size!=0) {
      cursor_outer(cursor, buffer);
      buffer_delete(buffer, cursor, size);
    }
    buffer_insert_size(buffer, cursor, "\n", 1);
    buffer_process_lines(buffer);
    break;
  case SDLK_TAB:
    buffer_insert_size(buffer, cursor, "    ", 4);
    buffer_process_lines(buffer);
    break;
  default:
    break;
  }

  window_fit(buffer, cursor);
}

//TODO: BLINKING CURSOR
int main(int argc, char **argv) {
  Cursor cursor = {0};
  Buffer buffer = {0};
  Text text = {0};

  buffer_init(&buffer);
  text_init(&text);

  text_read_file(&text, "./rsc/c.cfg");

  scc(SDL_Init(SDL_INIT_VIDEO));
  stcc(TTF_Init());

  if(argc>1) {
    buffer_load_file(&buffer, argv[1]);
  }
  
  SDL_Window *wind =
    scp(SDL_CreateWindow(
			 "Editor",
			 SDL_WINDOWPOS_CENTERED,
			 SDL_WINDOWPOS_CENTERED,
			 WIDTH, HEIGHT,
			 SDL_WINDOW_RESIZABLE
			 ));

  SDL_Renderer *rend =
    scp(SDL_CreateRenderer(
			   wind,
			   -1,
			   SDL_RENDERER_SOFTWARE
			   ));
  Font font;
  font_init(&font, rend, FONT_PATH, FONT_SIZE);

  width = WIDTH / font.width;
  height = HEIGHT / font.height;

  SDL_Event event;
  size_t d;

  PassingStruct s = {wind, rend, &font, &buffer, &cursor, &text, NULL};
  SDL_AddEventWatch(resizingEventWatcher, &s);

  buffer_process_lines(&buffer);

  while(running) {
    SDL_WaitEvent(&event);
    bool action = false;
    (void) action;

    switch(event.type) {
    case SDL_QUIT:
      running = false;
      break;
    case SDL_KEYDOWN:
      key_ctrl(event.key.keysym.sym, true);
      key_down(&buffer, &cursor, &font, rend, event.key.keysym.sym, argv[1]);
      action = true;
      break;
    case SDL_KEYUP:
      key_ctrl(event.key.keysym.sym, false);
      break;
    case SDL_TEXTINPUT:
      d = cursor_dist(&cursor, &buffer, NULL, NULL);
      if(d!=0) {
	cursor_outer(&cursor, &buffer);
	buffer_delete(&buffer, &cursor, d);
      }
      buffer_insert(&buffer, &cursor, event.text.text);
      buffer_process_lines(&buffer);
      text_update(&text, &buffer, &cursor, y, height);
      window_fit(&buffer, &cursor);
      action = true;      
      break;
    case SDL_MOUSEWHEEL:
      if(event.wheel.y>0) {
	y = y > 0 ? (y-1) : 0;
      }
      else if(event.wheel.y<0){
	y++;
      }
      break;
    case SDL_MOUSEMOTION:
      break;
    case SDL_MOUSEBUTTONDOWN:
      drag = true;
      cursor_reset(&cursor);
      if(event.button.button==SDL_BUTTON_LEFT) {
	cursor_click(&cursor, &buffer,
	             event.button.x + x*font.width - (off+1)*font.width,
	             event.button.y + y*font.height - PAD_TOP,
	             font.width, font.height, false);
      }
      break;
    case SDL_MOUSEBUTTONUP:
      if(event.button.button==SDL_BUTTON_LEFT) {
	cursor_click(&cursor, &buffer,
	             event.button.x + x*font.width - (off + 1)*font.width,
	             event.button.y + y*font.height - PAD_TOP,
	             font.width, font.height, true);
      }
      drag = false;
      break;
    default:
      break;
    }
    
    text_update(&text, &buffer, &cursor, y, height);
    render(rend, &font, &buffer, &cursor, &text, true);
  }

  SDL_DestroyRenderer(rend);
  SDL_DestroyWindow(wind);

  font_close(&font);

  text_quit(&text);
  buffer_quit(&buffer);  

  TTF_Quit();
  SDL_Quit();
  return 0;
}
