#include "jaunty/jaunty.h"
#include "herdem.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define DEBUG_MODE



void set_up_level_one()
{
    int map_w = 32, map_h = 22, tw = 32, th = 32;
    herdem_dog *dog;
    herdem_sheep *sheep;

    herdem_engine->target_sheeps = 2;
    herdem_engine->saved_sheeps = 0;
    herdem_engine->level_start_time = SDL_GetTicks();

    /* Creating info board */
    herdem_engine->info_board = new_herdem_info_board();

    /* Creating map */
    if(!(herdem_engine->parent.map = new_jty_map(
                map_w, map_h, tw, th,
                "images/map.png",
                "abcdefg"
                "hijklmn"
                "opqrstu",
                "abbbbbbbbbbbbbbbbbbbbbbbbbbbbbbc"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiir"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiik"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiiiiiiiiij"
                "oppppppppppppppppppppppppppppppq",
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
    dog->parent.x = dog->parent.px = 400;
    dog->parent.y = dog->parent.py = 300;
    dog->parent.vx = dog->parent.vy = 0.0;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 100;
    sheep->parent.y = sheep->parent.py = 300;
    sheep->parent.vx = 0.5;
    sheep->parent.vy = -0.5;

    sheep = new_herdem_sheep();
    sheep->parent.x = sheep->parent.px = 200;
    sheep->parent.y = sheep->parent.py = 200;
    sheep->parent.vx = 100;
    sheep->parent.vy = 100;
}

bool is_level_one_finished()
{
    herdem_engine->level_time = SDL_GetTicks() - herdem_engine->level_start_time;
    if (herdem_engine->saved_sheeps == herdem_engine->target_sheeps)
        return true;
    return false;
}

void clean_up_level_one()
{
    fprintf(stderr, "Level lasted %f seconds\n", herdem_engine->level_time/1000.);
    jty_engine->set_up_level = NULL;
}

int main(void)
{
    SDL_Event selection;
    int carry_on = 1;
    Uint32 start_t, curr_t;
    Uint32 c_frame = 0, p_frame = 0;
    int ef = 0;

    herdem_engine = new_herdem_eng();
    jty_engine->set_up_level = set_up_level_one;
    jty_engine->is_level_finished = is_level_one_finished;
    jty_engine->clean_up_level = clean_up_level_one;
    
    start_t = SDL_GetTicks();

    while (jty_engine->set_up_level != NULL && carry_on) {
        jty_engine->set_up_level();
        while(!jty_engine->is_level_finished() && carry_on){

            glClear(GL_COLOR_BUFFER_BIT);

            /* Do all the painting that is required */
            herdem_paint();
            p_frame++;

            glFlush();
            SDL_GL_SwapBuffers();

            /* See whether it is time for a logic frame */
            curr_t = SDL_GetTicks();
            herdem_engine->parent.elapsed_frames = ((double)(curr_t \
                        - start_t) / 1000. * FPS); 
            ef = (int)(herdem_engine->parent.elapsed_frames - c_frame);

            /* Work through all the logic frames */
            while(ef--){
                c_frame++;

                SDL_PumpEvents();
                if(SDL_PeepEvents(&selection, 1,
                            SDL_GETEVENT, SDL_EVENTMASK(SDL_QUIT))){
                    carry_on = 0;
                }

                herdem_iterate();

            }

            SDL_PumpEvents();
            if(SDL_PeepEvents(&selection, 1,
                        SDL_GETEVENT, SDL_EVENTMASK(SDL_QUIT))){
                carry_on = 0;
            }
        }
        jty_engine->clean_up_level();
    }

#ifdef DEBUG_MODE
    fprintf(stderr, "FPS (logic): %f\n", (float)c_frame/((float)(curr_t - start_t)/1000));
    fprintf(stderr, "FPS (rendered): %f\n", (float)p_frame/((float)(curr_t - start_t)/1000));
#endif

    free_herdem_eng(herdem_engine);

    return 0;
}
