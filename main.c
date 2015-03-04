#include "jaunty/jaunty.h"
#include "herdem.h"
#include "levels/levels.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define DEBUG_MODE

int main(void)
{
    SDL_Event selection;
    int carry_on = 1;
    Uint32 start_t, curr_t;
    Uint32 c_frame = 0, p_frame = 0;
    int ef = 0;

    herdem_engine = new_herdem_eng();

    /* Creating info board */
    herdem_engine->info_board = new_herdem_info_board();

    jty_engine->set_up_level = set_up_level_three;
    jty_engine->is_level_finished = is_level_three_finished;
    jty_engine->clean_up_level = clean_up_level_three;
    
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
