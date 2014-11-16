#ifndef KEYS_INCLUDED
#define KEYS_INCLUDED

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_LEFT 2
#define KEY_RIGHT 3
#define KEY_PLUS 4
#define KEY_MINUS 5
#define KEY_LEFT_BRACKET 6
#define KEY_RIGHT_BRACKET 7
#define KEY_ESCAPE 8
#define NUM_KEYS 9

void init_keys();
void process_events();
int key_down(int key_id);
int key_pressed(int key_id);
void finish_keys();


#endif
