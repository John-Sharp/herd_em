#include "jaunty/jaunty.h"
#include <stdio.h>
#define WIN_W 800
#define WIN_H 600
#define FPS 300

#define DEBUG_MODE

void test_m_handler(jty_actor *a, int i, int j, char tile_type)
{
    fprintf(stderr, "colliding with something\n");
    return;

}

void input_handler(struct jty_actor *actor)
{
    SDL_Event selection;

    if(SDL_PeepEvents(&selection, 1,
                SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN) |
                              SDL_EVENTMASK(SDL_KEYUP))){
        switch(selection.key.keysym.sym){
            case SDLK_UP:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vy = -0.5;
                else
                    actor->vy = 0;
                break;
            case SDLK_DOWN:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vy = 0.5;
                else
                    actor->vy = 0;
                break;
            case SDLK_LEFT:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vx = -0.5;
                else
                    actor->vx = 0;
                break;
            case SDLK_RIGHT:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vx = 0.5;
                else
                    actor->vx = 0;
                break;
            default:
                break;

        }
    }
}

int main(void)
{
    int map_w = 25, map_h = 18, tw = 32, th = 32;
    jty_actor *actor;
    SDL_Event selection;
    int carry_on = 1;
    Uint32 start_t, curr_t;
    Uint32 c_frame = 0, p_frame = 0;
    int ef = 0;

    jty_eng_create(WIN_W, WIN_H);
    
    /* Creating map */
    if(!(jty_engine.map = jty_new_map(
                map_w, map_h, tw, th,
                "images/map.png",
                "abcdefg"
                "hijklmn"
                "opqrstu",
                "ibbbbbbbbbbbibbbbbbbbbbbc"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "opppppppppppppppppppppppq",
                "bbbbbbbbbbbbbbbbbbbbbbbbb"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "bbbbbbbbbbbbbbbbbbbbbbbbb"
                ))) {
        fprintf(stderr, "Error loading map\n");
        return -1;
    }

    actor = jty_new_actor(104, 68, "images/sprites/dog/0.png");

    actor->x = actor->px = 400;
    actor->y = actor->py = 300;
    actor->vx = 0.0;

    jty_actor_add_i_handler(actor, input_handler);
    jty_actor_add_m_handler(actor, test_m_handler, "b");

    start_t = SDL_GetTicks();

    while(carry_on){

        glClear(GL_COLOR_BUFFER_BIT);

        /* Do all the painting that is required */
        jty_paint();
        p_frame++;

        glFlush();
        SDL_GL_SwapBuffers();

        /* See whether it is time for a logic frame */
        curr_t = SDL_GetTicks();
        jty_engine.elapsed_frames = ((double)(curr_t \
                    - start_t) / 1000. * FPS) - c_frame; 
        ef = (int)jty_engine.elapsed_frames;

        /* Work through all the logic frames */
        while(ef--){
            c_frame++;

            SDL_PumpEvents();
            if(SDL_PeepEvents(&selection, 1,
                        SDL_GETEVENT, SDL_EVENTMASK(SDL_QUIT))){
                carry_on = 0;
            }

            jty_iterate();

        }

        SDL_PumpEvents();
        if(SDL_PeepEvents(&selection, 1,
                    SDL_GETEVENT, SDL_EVENTMASK(SDL_QUIT))){
            carry_on = 0;
        }
    }

#ifdef DEBUG_MODE
    fprintf(stderr, "FPS (logic): %f\n", (float)c_frame/((float)(curr_t - start_t)/1000));
    fprintf(stderr, "FPS (rendered): %f\n", (float)p_frame/((float)(curr_t - start_t)/1000));
#endif

    jty_eng_free();

    return 0;
}
