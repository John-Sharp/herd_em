#include "jaunty/jaunty.h"
#include <stdio.h>
#define WIN_W 800
#define WIN_H 600
#define FPS 300

#define DEBUG_MODE

int dog_sprites[] = {
    /* top left */ 0,
    /* top centre */ 1,
    /* top right */ 2,
    /* center left */ 0,
    /* center center */ 1,
    /* center right */ 2,
    /* bottom left */ 0,
    /* bottom centre */ 3,
    /* bottom right */ 2, 

    /* stationary sprites */
    /* top left */ 4,
    /* top centre */ 5,
    /* top right */ 6,
    /* center left */ 4,
    /* center center */ 5,
    /* center right */ 6,
    /* bottom left */ 4,
    /* bottom centre */ 7,
    /* bottom right */ 6 
};

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
        int old_index = actor->vx + 1  + 3 * (actor->vy + 1);

        switch(selection.key.keysym.sym){
            case SDLK_UP:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vy = -1;
                else
                    actor->vy = 0;
                break;
            case SDLK_DOWN:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vy = 1;
                else
                    actor->vy = 0;
                break;
            case SDLK_LEFT:
                if(selection.key.type == SDL_KEYDOWN)
                    actor->vx = -1;
                else
                    actor->vx = 0;
                break;
            case SDLK_RIGHT:
                if(selection.key.type == SDL_KEYDOWN) {
                    actor->vx = 1;
                }   else
                    actor->vx = 0;
                break;
            default:
                break;
        }

        int index = actor->vx + 1 + 3 * (actor->vy + 1);

        /* if actor currently stationary... */
        if(index == 4) {
            index = 9 + old_index;
        }

        actor->current_sprite = dog_sprites[index];
    }
}

void animation_handler(struct jty_actor *actor) 
{
    int animation_framerate = 8; /* frames per second */
    jty_sprite *sprite = actor->sprites[actor->current_sprite];
    int animation_frames_passed = jty_engine.elapsed_frames / FPS * animation_framerate;
    actor->current_frame = animation_frames_passed % sprite->num_of_frames;
    return;
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

    actor = jty_new_actor(
            8,
            104, 68, "images/sprites/dog/left_walking.png", "images/c_sprites/dog/left_walking.png",
            68, 104, "images/sprites/dog/up_walking.png", "images/c_sprites/dog/up_walking.png",
            104, 68, "images/sprites/dog/right_walking.png", "images/c_sprites/dog/right_walking.png",
            68, 104, "images/sprites/dog/down_walking.png", "images/c_sprites/dog/down_walking.png",
            104, 68, "images/sprites/dog/left_still.png", "images/c_sprites/dog/left_still.png",
            68, 104, "images/sprites/dog/up_still.png", "images/c_sprites/dog/up_still.png",
            104, 68, "images/sprites/dog/right_still.png", "images/c_sprites/dog/right_still.png",
            68, 104, "images/sprites/dog/down_still.png", "images/c_sprites/dog/down_still.png"
            );

    actor->x = actor->px = 400;
    actor->y = actor->py = 300;
    actor->vx = 0.0;

    jty_actor_add_i_handler(actor, input_handler);
    jty_actor_add_i_handler(actor, animation_handler);
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
                    - start_t) / 1000. * FPS); 
        ef = (int)(jty_engine.elapsed_frames - c_frame);

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
