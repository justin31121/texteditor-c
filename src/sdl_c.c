#include "./sdl_c.h"

int scc(int code) {
    if(code<0) {
        fprintf(stderr, "SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }  
    return code;
}

int stcc(int code) {
    if(code<0) {
        fprintf(stderr, "TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }  
    return code;
}

void *scp(void* ptr) {
    if(!ptr) {
        fprintf(stderr, "SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

void *stcp(void *ptr) {
    if(!ptr) {
        fprintf(stderr, "TTF_Error: %s\n", TTF_GetError());
        exit(1);
    }
    return ptr;
}
