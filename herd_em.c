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

void sheep_wall_handler(jty_actor *a, int i, int j, char tile_type)
{
    int tw = jty_engine.map->tw;
    int th = jty_engine.map->th;

    if(a->x >= i * tw && a->x < (i + 1) * tw) {
        fprintf(stderr, "TTTTHERRREEEE!\n");
        a->vx *= -1;
    }

    if(a->y >= j * th && a->y < (j + 1) * th) {
        fprintf(stderr, "HERRREEEE!\n");
        a->vy *= -1;
    }


    return;
}

void dog_wall_handler(jty_actor *a, int i, int j, char tile_type, jty_c_info *c_info)
{
    int tw = jty_engine.map->tw;
    int th = jty_engine.map->th;

    if(a->x >= i * tw && a->x < (i + 1) * tw) {
        if(a->y > j * th) {
            if(a->vy < 0)
                a->y = a->py;
        }else {
            if(a->vy > 0)
                a->y = a->py;
        }
    }

    if(a->y >= j * th && a->y < (j + 1) * th) {
        if(a->x > i * tw) {
            if(a->vx < 0)
                a->x = a->px;
        }else {
            if(a->vx > 0)
                a->x = a->px;
        }
    }

    return;
}

void rectangle_test_handler(jty_actor *a, int i, int j, char tile_type, jty_c_info *c_info)
{
    a->x -= c_info->normal.x * (c_info->penetration + 1);
    a->y -= c_info->normal.y * (c_info->penetration + 1);

    if (c_info->normal.x)
        a->vx = 0;
    if (c_info->normal.y)
        a->vy = 0;
}

void red_green_collision_handler(jty_actor *a1, jty_actor *a2, jty_c_info *c_info)
{
    a1->x -= c_info->normal.x * (c_info->penetration);
    a1->y -= c_info->normal.y * (c_info->penetration);

    fprintf(stderr, "actor collison, actor1: %d\n", a1->uid);

    print_c_info(c_info);

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
        index = 0;

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

//    actor = jty_new_actor(
//            DOGS,
//            8,
//            104, 68, "images/sprites/dog/left_walking.png", "images/c_sprites/dog/left_walking.png",
//            68, 104, "images/sprites/dog/up_walking.png", "images/c_sprites/dog/up_walking.png",
//            104, 68, "images/sprites/dog/right_walking.png", "images/c_sprites/dog/right_walking.png",
//            68, 104, "images/sprites/dog/down_walking.png", "images/c_sprites/dog/down_walking.png",
//            104, 68, "images/sprites/dog/left_still.png", "images/c_sprites/dog/left_still.png",
//            68, 104, "images/sprites/dog/up_still.png", "images/c_sprites/dog/up_still.png",
//            104, 68, "images/sprites/dog/right_still.png", "images/c_sprites/dog/right_still.png",
//            68, 104, "images/sprites/dog/down_still.png", "images/c_sprites/dog/down_still.png"
//            );
    jty_shape test_c_shape = {.centre = {.x = 0, .y = 0}, .radius = 0, .w = 40, .h = 40, .type = JTY_RECT};
    jty_shape test_c_shape2 = {.centre = {.x = 0, .y = 0}, .radius = 0, .w = 104, .h = 68, .type = JTY_RECT};
    jty_shape *test_c_shapes[2] = {&test_c_shape, &test_c_shape2};
    actor = jty_new_actor(
            DOGS,
            1,
            104, 68, "images/sprites/test/fortyfortysquare.png", test_c_shapes);

    actor->x = actor->px = 400;
    actor->y = actor->py = 300;
    actor->vx = 0.0;

    jty_actor_add_i_handler(actor, input_handler);
    //jty_actor_add_i_handler(actor, animation_handler);
    //jty_actor_add_m_handler(actor, dog_wall_handler, "b");
    jty_actor_add_m_handler(actor, rectangle_test_handler, "b");


    jty_shape sheep_c_shape = {.centre = {.x = 0, .y = 0}, .radius = 0, .w = 40, .h = 60, .type = JTY_RECT};
    jty_shape *sheep_c_shapes[] = {&sheep_c_shape};
    sheep = jty_new_actor(
            SHEEP,
            1,
            100, 100, "images/sprites/test/fortysixtysquare.png", sheep_c_shapes);
    sheep->x = sheep->px = 200;
    sheep->y = sheep->py = 200;


    jty_eng_add_a_a_handler(DOGS, SHEEP, red_green_collision_handler);
//    sheep = jty_new_actor(
//           SHEEP,
//           16,
//           124, 124, "images/sprites/sheep/NW_walking.png", "images/c_sprites/sheep/NW_walking.png",
//           69, 106, "images/sprites/sheep/N_walking.png", "images/c_sprites/sheep/N_walking.png",
//           124, 124, "images/sprites/sheep/NE_walking.png", "images/c_sprites/sheep/NE_walking.png",
//           106, 69, "images/sprites/sheep/E_walking.png", "images/c_sprites/sheep/E_walking.png",
//           124, 124, "images/sprites/sheep/SE_walking.png", "images/c_sprites/sheep/SE_walking.png",
//           69, 106, "images/sprites/sheep/S_walking.png", "images/c_sprites/sheep/S_walking.png",
//           124, 124, "images/sprites/sheep/SW_walking.png", "images/c_sprites/sheep/SW_walking.png",
//           106, 69, "images/sprites/sheep/W_walking.png", "images/c_sprites/sheep/W_walking.png",
//
//           124, 124, "images/sprites/sheep/NW_still.png", "images/c_sprites/sheep/NW_still.png",
//           69, 106, "images/sprites/sheep/N_still.png", "images/c_sprites/sheep/N_still.png",
//           124, 124, "images/sprites/sheep/NE_still.png", "images/c_sprites/sheep/NE_still.png",
//           106, 69, "images/sprites/sheep/E_still.png", "images/c_sprites/sheep/E_still.png",
//           124, 124, "images/sprites/sheep/SE_still.png", "images/c_sprites/sheep/SE_still.png",
//           69, 106, "images/sprites/sheep/S_still.png", "images/c_sprites/sheep/S_still.png",
//           124, 124, "images/sprites/sheep/SW_still.png", "images/c_sprites/sheep/SW_still.png",
//           106, 69, "images/sprites/sheep/W_still.png", "images/c_sprites/sheep/W_still.png"
//           );
//
//    jty_actor_add_i_handler(sheep, animation_handler);
//    jty_actor_add_m_handler(sheep, sheep_wall_handler, "b");
//
//    sheep->x = sheep->px = 400;
//    sheep->y = sheep->py = 300;
//    sheep->vx = 1;
//    sheep->vy = 0;


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
