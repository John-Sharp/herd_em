#include "levels.h"
#include "../herdem.h"
#include "../jaunty/jaunty.h"

void set_up_inter_level(void (*clean_up_function)())
{
    int map_w = 1, map_h = 1, tw = WIN_W, th = WIN_H;

    if(!(herdem_engine->parent.map = new_jty_map(
                    map_w, map_h, tw, th,
                    "images/scenes/level_intro.png",
                    "a",
                    "a",
                    "a"))) {
        fprintf(stderr, "Error loading map\n");
        exit(1);
    }

    jty_engine->is_level_finished = is_inter_level_finished;
    jty_engine->clean_up_level = clean_up_function;
}

void clean_up_inter_level(void (*next_level_function)())
{
    free_jty_map(jty_engine->map);
    jty_engine->map = NULL;

    jty_engine->set_up_level = next_level_function;
}

void set_up_inter_level_one()
{
    set_up_inter_level(clean_up_inter_level_one);
}

void set_up_inter_level_two()
{
    set_up_inter_level(clean_up_inter_level_two);
}

void set_up_inter_level_three()
{
    set_up_inter_level(clean_up_inter_level_three);
}

void set_up_inter_level_four()
{
    set_up_inter_level(clean_up_inter_level_four);
}

void set_up_inter_level_five()
{
    set_up_inter_level(clean_up_inter_level_five);
}

void set_up_inter_level_six()
{
    set_up_inter_level(clean_up_inter_level_six);
}

bool is_inter_level_finished()
{
    SDL_Event selection;
    if(SDL_PeepEvents(&selection, 1,
                SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN))){
        return true;
    }

    return false;
}

void clean_up_inter_level_one()
{
    clean_up_inter_level(set_up_level_two);
}

void clean_up_inter_level_two()
{
    clean_up_inter_level(set_up_level_three);
}

void clean_up_inter_level_three()
{
    clean_up_inter_level(set_up_level_four);
}

void clean_up_inter_level_four()
{
    clean_up_inter_level(set_up_level_five);
}

void clean_up_inter_level_five()
{
    clean_up_inter_level(set_up_level_six);
}

void clean_up_inter_level_six()
{
    clean_up_inter_level(set_up_level_seven);
}
