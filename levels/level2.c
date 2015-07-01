#include "levels.h"
#include "../herdem.h"
#include "../jaunty/jaunty.h"

void set_up_level_two()
{
    int map_w = 32, map_h = 22, tw = 32, th = 32;
    herdem_dog *dog;
    herdem_sheep *sheep;

    herdem_engine->target_sheeps = 4;
    herdem_engine->saved_sheeps = 0;
    herdem_engine->time_limit = 19 * 1000;
    herdem_engine->level_start_time = SDL_GetTicks();
    herdem_saved_tally_update(herdem_engine->info_board->saved_tally);

    jty_engine->is_level_finished = is_level_two_finished;
    jty_engine->clean_up_level = clean_up_level_two;

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
    sheep->parent.x = sheep->parent.px = 115;
    sheep->parent.y = sheep->parent.py = 560;
    sheep->parent.vx = 0.1;
    sheep->parent.vy = 0.5;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 540;
    sheep->parent.y = sheep->parent.py = 350;
    sheep->parent.vx = -0.5;
    sheep->parent.vy = -0.1;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 115;
    sheep->parent.y = sheep->parent.py = 95;
    sheep->parent.vx = -0.5;
    sheep->parent.vy = -0.5;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 215;
    sheep->parent.y = sheep->parent.py = 480;
    sheep->parent.vx = -0.5;
    sheep->parent.vy = -0.5;


}

bool is_level_two_finished()
{
    herdem_engine->level_time = SDL_GetTicks() - herdem_engine->level_start_time;
    if (herdem_engine->saved_sheeps == herdem_engine->target_sheeps ||
        herdem_engine->time_limit < herdem_engine->level_time)
        return true;
    return false;
}

void clean_up_level_two()
{
    fprintf(stderr, "Level lasted %f seconds\n", herdem_engine->level_time/1000.);

    herdem_eng_clean_up_level(herdem_engine);

    if (herdem_engine->time_limit < herdem_engine->level_time)
        jty_engine->set_up_level = set_up_level_two;
    else 
        jty_engine->set_up_level = set_up_inter_level_two;
}

