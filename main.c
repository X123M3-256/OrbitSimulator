#include<stdio.h>
#include<stdlib.h>
#include<SDL/SDL.h>
#include<math.h>
#include "sim.h"
#include "keys.h"

int running=1;

void init(int width,int height)
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
    printf("Failed to initialize SDL\n");
    exit(0);
    }
atexit(SDL_Quit);
draw_init(width,height);
}


void render(simulation_t* sim)
{
draw_begin();
draw_clear();
simulation_render(sim);
draw_end();
}


int main()
{
init(1000,700);

simulation_t sim;
create_simulation(&sim);

generate_solar_system(&sim);
spacecraft_generate(&sim);


init_physics(&sim);

	while(!key_pressed(KEY_ESCAPE))
	{
    process_events();
    simulation_process_input(&sim);
    run_physics(&sim);
    render(&sim);
	usleep(20000);
	}

return 0;
}
