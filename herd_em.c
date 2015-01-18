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

enum {
    DIRECTION_NW, DIRECTION_N, DIRECTION_NE,
    DIRECTION_E, DIRECTION_SE, DIRECTION_S,
    DIRECTION_SW, DIRECTION_W
};

int sheep_sprites[] = {
    /* NW */ 0,
    /* N */ 1,
    /* NE */ 2,
    /* E */ 3,
    /* SE */ 4,
    /* S */ 5,
    /* SW */ 6,
    /* W */ 7,

    /* stationary sprites */
    /* NW */ 8,
    /* N */ 9,
    /* NE */ 10,
    /* E */ 11,
    /* SE */ 12,
    /* S */ 13,
    /* SW */ 14,
    /* W */ 15
};

enum { DOGS = 1, SHEEP = 1<<1 };

/**
 * Sets the current sprite of actor `a` so that
 * it corresponds to its velocity (assuming there
 * are 8 different direction spirtes
 */
void eight_way_direction_change(jty_actor *a)
{
    /* b = (cos(22.5))**2 */
    double b = 0.8535533905932737;
    double x, y;

    x = 1 / (1 + a->vy * a->vy / (a->vx * a->vx));
    if (a->vx == 0 && a->vy == 0) {
        a->current_sprite += 8;
        return;
    }
    if (x > b) {
        if (a->vx > 0) {
            /* Travelling E */
            fprintf(stderr, "travelling' E\n");
            a->current_sprite = DIRECTION_E;
            return;
        } else {
            fprintf(stderr, "travelling' W\n");
            /* Travelling W */
            a->current_sprite = DIRECTION_W;
            return;
        }
    }

    y = 1 / (1 + a->vx * a->vx / (a->vy * a->vy));
    if (y > b) {
        if (a->vy < 0) {
            /* Travelling N */
            a->current_sprite = DIRECTION_N;
            return;
        } else {
            /* Travelling S */
            a->current_sprite = DIRECTION_S;
            return;
        }
    }

    if (a->vx > 0 && a->vy < 0) {
        /* Travelling NE */
        a->current_sprite = DIRECTION_NE;
        return;
    }

    if (a->vx < 0 && a->vy < 0) {
        /* Travelling NW */
        a->current_sprite = DIRECTION_NW;
        return;
    }

    if (a->vx < 0 && a->vy > 0) {
        /* Travelling SW */
        a->current_sprite = DIRECTION_SW;
        return;
    }

    a->current_sprite = DIRECTION_SE;

}

void sheep_wall_handler(jty_actor *a, int i, int j, char tile_type, jty_c_info *c_info)
{
    a->x -= c_info->normal.x * (c_info->penetration + 1);
    a->y -= c_info->normal.y * (c_info->penetration + 1);

    if (c_info->normal.x)
        a->vx *= -1;
    if (c_info->normal.y)
        a->vy *= -1;
    eight_way_direction_change(a);
}

void dog_wall_handler(jty_actor *a, int i, int j, char tile_type, jty_c_info *c_info)
{
    a->x -= c_info->normal.x * (c_info->penetration + 1);
    a->y -= c_info->normal.y * (c_info->penetration + 1);
}

void dog_sheep_collision_handler(jty_actor *dog, jty_actor *sheep, jty_c_info *c_info)
{
    jty_vector v_rel = {sheep->vx - dog->vx, sheep->vy - dog->vy};
    sheep->x += c_info->normal.x * c_info->penetration;
    sheep->y += c_info->normal.y * c_info->penetration;

    if (c_info->normal.x)
        v_rel.x *= -1;
    if (c_info->normal.y)
        v_rel.y *= -1;

    sheep->vx = v_rel.x;
    sheep->vy = v_rel.y;
    eight_way_direction_change(sheep);
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

        eight_way_direction_change(actor);
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
    jty_actor *actor, *sheep;
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

   jty_shape herd_em_c_shape = {.centre = {.x = 0, .y = 0}, .radius = 0, .w = 40, .h = 40, .type = JTY_RECT};
   jty_shape *herd_em_c_shapes[4] = {&herd_em_c_shape, &herd_em_c_shape, &herd_em_c_shape, &herd_em_c_shape};
   actor = jty_new_actor(
           DOGS,
           16,
           127, 127, "images/sprites/dog/NW_walking.png", herd_em_c_shapes,
           71, 108, "images/sprites/dog/N_walking.png", herd_em_c_shapes,
           127, 127, "images/sprites/dog/NE_walking.png", herd_em_c_shapes,
           108, 71, "images/sprites/dog/E_walking.png", herd_em_c_shapes,
           127, 127, "images/sprites/dog/SE_walking.png", herd_em_c_shapes,
           71, 108, "images/sprites/dog/S_walking.png", herd_em_c_shapes,
           127, 127, "images/sprites/dog/SW_walking.png", herd_em_c_shapes,
           108, 71, "images/sprites/dog/W_walking.png", herd_em_c_shapes,

           127, 127, "images/sprites/dog/NW_walking.png", herd_em_c_shapes,
           71, 108, "images/sprites/dog/N_still.png", herd_em_c_shapes,
           127, 127, "images/sprites/dog/NE_still.png", herd_em_c_shapes,
           108, 71, "images/sprites/dog/E_still.png", herd_em_c_shapes,
           127, 127, "images/sprites/dog/SE_still.png", herd_em_c_shapes,
           71, 108, "images/sprites/dog/S_still.png", herd_em_c_shapes,
           127, 127, "images/sprites/dog/SW_still.png", herd_em_c_shapes,
           108, 71, "images/sprites/dog/W_still.png", herd_em_c_shapes

           );

    actor->x = actor->px = 400;
    actor->y = actor->py = 300;
    actor->vx = actor->vy = 0.0;

    jty_actor_add_i_handler(actor, input_handler);
    jty_actor_add_i_handler(actor, animation_handler);
    jty_actor_add_m_handler(actor, dog_wall_handler, "b");

    sheep = jty_new_actor(
            SHEEP,
            16,
            124, 124, "images/sprites/sheep/NW_walking.png", herd_em_c_shapes,
            69, 106, "images/sprites/sheep/N_walking.png", herd_em_c_shapes,
            124, 124, "images/sprites/sheep/NE_walking.png", herd_em_c_shapes,
            106, 69, "images/sprites/sheep/E_walking.png", herd_em_c_shapes,
            124, 124, "images/sprites/sheep/SE_walking.png", herd_em_c_shapes,
            69, 106, "images/sprites/sheep/S_walking.png", herd_em_c_shapes,
            124, 124, "images/sprites/sheep/SW_walking.png", herd_em_c_shapes,
            106, 69, "images/sprites/sheep/W_walking.png", herd_em_c_shapes,

            124, 124, "images/sprites/sheep/NW_still.png", herd_em_c_shapes,
            69, 106, "images/sprites/sheep/N_still.png", herd_em_c_shapes,
            124, 124, "images/sprites/sheep/NE_still.png", herd_em_c_shapes,
            106, 69, "images/sprites/sheep/E_still.png", herd_em_c_shapes,
            124, 124, "images/sprites/sheep/SE_still.png", herd_em_c_shapes,
            69, 106, "images/sprites/sheep/S_still.png", herd_em_c_shapes,
            124, 124, "images/sprites/sheep/SW_still.png", herd_em_c_shapes,
            106, 69, "images/sprites/sheep/W_still.png", herd_em_c_shapes
            );
    jty_actor_add_i_handler(sheep, animation_handler);
    jty_actor_add_m_handler(sheep, sheep_wall_handler, "b");
    
    sheep->x = sheep->px = 100;
    sheep->y = sheep->py = 100;
    sheep->vx = 1;
    sheep->vy = 0;
    eight_way_direction_change(sheep);

    jty_eng_add_a_a_handler(DOGS, SHEEP, dog_sheep_collision_handler);

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
