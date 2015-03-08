#include "levels.h"
#include "../herdem.h"
#include "../jaunty/jaunty.h"

void set_up_level_five()
{
    int map_w = 32, map_h = 22, tw = 32, th = 32;
    herdem_dog *dog;
    herdem_sheep *sheep;

    herdem_engine->target_sheeps = 6;
    herdem_engine->saved_sheeps = 0;
    herdem_engine->level_start_time = SDL_GetTicks();
    herdem_saved_tally_update(herdem_engine->info_board->saved_tally);

    jty_engine->is_level_finished = is_level_five_finished;
    jty_engine->clean_up_level = clean_up_level_five;

    /* Creating map */
    if(!(herdem_engine->parent.map = new_jty_map(
                map_w, map_h, tw, th,
                "images/map.png",
                "ABCDEFGHIJK"
                "LMNOPQRSTUV"
                "WXYZabcdefg",
                "ABBBBBBBBBbBBBBBBBBbBBBBBBbBBBBC"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMaMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMMMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMMMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMMMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMMMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMPMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMZMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMMMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMMMMMMN"
                "LMMMMMMMMMaMMMMMMMMLMMMMMMMMMMMN"
                "LMMMMMMMMMMMMMMMMMMLMMMMMMOMMMMN"
                "LMMMMMMMMMMMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMMMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMMMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMPMMMMMMMMLMMMMMMNMMMMN"
                "LMMMMMMMMMLMMMMMMMMLMMMMMMNMMMMN"
                "WXXXXXXXXXQXXXXXXXXQXXXXXXQEMMDY",

                "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaaaaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaaaaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaaaaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaaaaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaaaaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaaaaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaaaaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaaaaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaaaaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "baaaaaaaaabaaaaaaaabaaaaaabaaaab"
                "bbbbbbbbbbbbbbbbbbbbbbbbbbbbccbb"
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
    dog->parent.x = dog->parent.px = 920;
    dog->parent.y = dog->parent.py = 635;
    dog->parent.vx = dog->parent.vy = 0.0;
    dog->parent.ax = dog->parent.ay = 0.0;
    dog->parent.current_sprite = DIRECTION_N;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 95;
    sheep->parent.y = sheep->parent.py = 125;
    sheep->parent.vx = -0.5;
    sheep->parent.vy = -0.5;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 230;
    sheep->parent.y = sheep->parent.py = 95;
    sheep->parent.vx = 1;
    sheep->parent.vy = 0.0;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 550;
    sheep->parent.y = sheep->parent.py = 265;
    sheep->parent.vx = -0.5;
    sheep->parent.vy = -0.5;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 405;
    sheep->parent.y = sheep->parent.py = 460;
    sheep->parent.vx = 0.5;
    sheep->parent.vy = 0.5;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 780;
    sheep->parent.y = sheep->parent.py = 100;
    sheep->parent.vx = 0.0;
    sheep->parent.vy = 1;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 680;
    sheep->parent.y = sheep->parent.py = 620;
    sheep->parent.vx = 0.9;
    sheep->parent.vy = -0.1;
}

bool is_level_five_finished()
{
    herdem_engine->level_time = SDL_GetTicks() - herdem_engine->level_start_time;
    if (herdem_engine->saved_sheeps == herdem_engine->target_sheeps)
        return true;
    return false;
}

void clean_up_level_five()
{
    fprintf(stderr, "Level lasted %f seconds\n", herdem_engine->level_time/1000.);
    jty_engine->set_up_level = NULL;
}

