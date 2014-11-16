#include<stdio.h>
#include<stdlib.h>
#include<SDL/SDL.h>
#include "keys.h"

char pressed_keys[NUM_KEYS];
char keys[NUM_KEYS];


void init_keys()
{
}
void finish_keys()
{
}


void process_key_event(SDL_Event* event)
{
int key=-1;
    switch(event->key.keysym.sym)
    {
    case SDLK_UP:
    key=KEY_UP;
    break;
    case SDLK_DOWN:
    key=KEY_DOWN;
    break;
    case SDLK_LEFT:
    key=KEY_LEFT;
    break;
    case SDLK_RIGHT:
    key=KEY_RIGHT;
    break;
    case SDLK_PLUS:
    case SDLK_EQUALS:
    key=KEY_PLUS;
    break;
    case SDLK_MINUS:
    case SDLK_UNDERSCORE:
    key=KEY_MINUS;
    break;
    case SDLK_LEFTBRACKET:
    case SDLK_COMMA:
    key=KEY_LEFT_BRACKET;
    break;
    case SDLK_RIGHTBRACKET:
    case SDLK_PERIOD:
    key=KEY_RIGHT_BRACKET;
    break;
    case SDLK_ESCAPE:
    key=KEY_ESCAPE;
    break;
    }
    if(key<0)return;

    if(event->type==SDL_KEYDOWN)
    {
    pressed_keys[key]=1;
    keys[key]=1;
    }
    else if(event->type==SDL_KEYUP)keys[key]=0;
}

void process_events()
{
SDL_Event event;
//Resert pressed_keys
memset(pressed_keys,0,NUM_KEYS);
//Handle events
    while(SDL_PollEvent(&event))
    {
        if(event.type==SDL_KEYDOWN||event.type==SDL_KEYUP)process_key_event(&event);
    }
}



int key_pressed(int key_id)
{
return (int)pressed_keys[key_id];
}

int key_down(int key_id)
{
return (int)keys[key_id];
}
