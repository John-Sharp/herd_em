#include "levels.h"
#include "../herdem.h"
#include "../jaunty/jaunty.h"

void set_up_level_one()
{
    int map_w = 32, map_h = 22, tw = 32, th = 32;
    herdem_dog *dog;
    herdem_sheep *sheep;

    herdem_engine->target_sheeps = 2;
    herdem_engine->saved_sheeps = 0;
    herdem_engine->level_start_time = SDL_GetTicks();
    herdem_engine->time_limit = 16 * 1000;

    /* Creating map */
    if(!(herdem_engine->parent.map = new_jty_map(
                map_w, map_h, tw, th,
                "images/map.png",
                "ABCDEFGHIJK"
                "LMNOPQRSTUV"
                "WXYZabcdefg",
                "ABBBBBBBBBBBBBBBBBBBBBBBBBBBBBBC"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMZ"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMM"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMO"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMMMMMMMMMMMMMN"
                "WXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXY",

                "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaaaaaaaaab"
                "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                ))) {
        fprintf(stderr, "Error loading map\n");
        exit(1);
    }

    jty_map_add_a_a_handler(
            jty_engine->map,
            HERDEM_SHEEP_GROUP_NUM,
            HERDEM_SHEEP_GROUP_NUM,
            herdem_sheep_sheep_collision_handler);
    jty_map_add_a_a_handler(
            jty_engine->map,
            HERDEM_DOG_GROUP_NUM,
            HERDEM_SHEEP_GROUP_NUM,
            herdem_dog_sheep_collision_handler);

    dog = new_herdem_dog();
    dog->parent.x = dog->parent.px = 940;
    dog->parent.y = dog->parent.py = 258;
    dog->parent.vx = dog->parent.vy = 0.0;
    dog->parent.ax = dog->parent.ay = 0.0;
    dog->parent.current_sprite = DIRECTION_W;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 200;
    sheep->parent.y = sheep->parent.py = 137;
    sheep->parent.vx = -0.5;
    sheep->parent.vy = -0.5;
    sheep->parent.ax = sheep->parent.ay = 0;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 600;
    sheep->parent.y = sheep->parent.py = 622;
    sheep->parent.vx = -0.9;
    sheep->parent.vy = 0.2;
    sheep->parent.ax = sheep->parent.ay = 0;
}

bool is_level_one_finished()
{
    herdem_engine->level_time = SDL_GetTicks() - herdem_engine->level_start_time;
    if (
            herdem_engine->saved_sheeps == herdem_engine->target_sheeps ||
            herdem_engine->time_limit < herdem_engine->level_time
       )
        return true;
    return false;
}

void clean_up_level_one()
{
    fprintf(stderr, "Level lasted %f seconds\n", herdem_engine->level_time/1000.);

    if (herdem_engine->time_limit < herdem_engine->level_time)
        jty_engine->set_up_level = set_up_level_one;
    else 
        jty_engine->set_up_level = set_up_level_two;
}

