#ifndef DRAW_INCLUDED
#define DRAW_INCLUDED
#include <SDL/SDL.h>
typedef Uint32 color_t;

void draw_init();
void draw_begin();
void draw_end();
void draw_clear();
color_t get_color(unsigned char r,unsigned char g,unsigned char b);
void set_pixel(unsigned int x,unsigned int y,color_t color);
void draw_line(unsigned int x0, unsigned int y0,unsigned int x1,unsigned int y1,color_t color);
void draw_text(unsigned int x,unsigned int y,const char* text);
unsigned int draw_get_width();
unsigned int draw_get_height();
#endif
