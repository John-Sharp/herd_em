#include "jaunty/jaunty.h"
#include <stdio.h>
#include <stdbool.h>
#define WIN_W 800
#define WIN_H 600
#define FPS 300

#define DEBUG_MODE

enum {
    DIRECTION_NW, DIRECTION_N, DIRECTION_NE,
    DIRECTION_E, DIRECTION_SE, DIRECTION_S,
    DIRECTION_SW, DIRECTION_W
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
        if (a->current_sprite < 8) {
            a->current_sprite += 8;
        }
        return;
    }

    if (x > b) {
        if (a->vx > 0) {
            /* Travelling E */
            a->current_sprite = DIRECTION_E;
            return;
        } else {
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


typedef enum herdem_player_action {
    HERDEM_MOVE_N = 1,
    HERDEM_MOVE_E = 1<<1,
    HERDEM_MOVE_S = 1<<2,
    HERDEM_MOVE_W = 1<<3} herdem_player_action;

typedef struct herdem_dog {
    jty_actor actor;
    double normal_speed;
    double scare_radius_min; /* radius where sheep run away with their max. speed */
    double scare_radius_max; /* maximum radius where sheep can see dog */
    herdem_player_action player_actions;
}herdem_dog;

void herdem_dog_calculate_velocity(herdem_dog *dog)
{
    /* 1/sqrt(2) */
    double s2r = 0.7071067811865475;

    dog->actor.vx = 0;
    dog->actor.vy = 0;

    if (dog->player_actions & HERDEM_MOVE_N) {
        if (dog->player_actions & HERDEM_MOVE_S) {
            dog->actor.vy = 0;
        }
        dog->actor.vy = -dog->normal_speed;
    } else if (dog->player_actions & HERDEM_MOVE_S) {
        dog->actor.vy = dog->normal_speed;
    }

    if (dog->player_actions & HERDEM_MOVE_E) {
        if (dog->player_actions & HERDEM_MOVE_W) {
            dog->actor.vx = 0;
        }
        if (dog->actor.vy != 0) {
            dog->actor.vx = s2r * dog->normal_speed;
            dog->actor.vy *= s2r;
        } else {
            dog->actor.vx = dog->normal_speed;
        }
    } else if (dog->player_actions & HERDEM_MOVE_W) {
        if (dog->actor.vy != 0) {
            dog->actor.vx = -s2r * dog->normal_speed;
            dog->actor.vy *= s2r;
        } else {
            dog->actor.vx = -dog->normal_speed;
        }
    }
    eight_way_direction_change((jty_actor *)dog);
}

void input_handler(struct jty_actor *actor)
{
    SDL_Event selection;
    struct herdem_dog *dog = (herdem_dog *)actor;

    if(SDL_PeepEvents(&selection, 1,
                SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN) |
                              SDL_EVENTMASK(SDL_KEYUP))){

        switch(selection.key.keysym.sym){
            case SDLK_UP:
                if(selection.key.type == SDL_KEYDOWN) {
                    dog->player_actions |= HERDEM_MOVE_N;
                } else
                    dog->player_actions ^= HERDEM_MOVE_N;
                break;
            case SDLK_DOWN:
                if(selection.key.type == SDL_KEYDOWN)
                    dog->player_actions |= HERDEM_MOVE_S;
                else
                    dog->player_actions ^= HERDEM_MOVE_S;
                break;
            case SDLK_LEFT:
                if(selection.key.type == SDL_KEYDOWN)
                    dog->player_actions |= HERDEM_MOVE_W;
                else
                    dog->player_actions ^= HERDEM_MOVE_W;
                break;
            case SDLK_RIGHT:
                if(selection.key.type == SDL_KEYDOWN) {
                    dog->player_actions |= HERDEM_MOVE_E;
                }   else
                    dog->player_actions ^= HERDEM_MOVE_E;
                break;
            default:
                break;
        }
        herdem_dog_calculate_velocity(dog);
    }
}



void animation_handler(struct jty_actor *actor) 
{
    int animation_framerate = 8; /* frames per second */
    jty_sprite *sprite = actor->sprites[actor->current_sprite];
    int animation_frames_passed = jty_engine->elapsed_frames / FPS * animation_framerate;
    actor->current_frame = animation_frames_passed % sprite->num_of_frames;
    return;
}

typedef struct herdem_eng {
    jty_eng main_engine;

    jty_actor_ls *sheeps;
    jty_shape *sheep_c_shape;
    jty_shape **sheep_c_shapes;

    jty_actor_ls *dogs;
    jty_shape *dog_c_shape;
    jty_shape **dog_c_shapes;
} herdem_eng;

herdem_eng *herdem_engine;

herdem_eng *new_herdem_eng()
{
    herdem_eng *hep;
    int i = 0;

    hep = realloc(jty_eng_create(WIN_W, WIN_H), sizeof(*hep));
    jty_engine = (jty_eng *)hep;

    hep->sheeps = NULL;
    hep->sheep_c_shape = malloc(sizeof(*hep->sheep_c_shape));
    hep->sheep_c_shape->centre.x = 0;
    hep->sheep_c_shape->centre.y = 0;
    hep->sheep_c_shape->radius =0;
    hep->sheep_c_shape->w = 40;
    hep->sheep_c_shape->h = 40;
    hep->sheep_c_shape->type = JTY_RECT;

    hep->sheep_c_shapes = malloc(4*sizeof(*hep->sheep_c_shapes));
    for (i = 0; i < 4; i++) {
        hep->sheep_c_shapes[i] = hep->sheep_c_shape;
    }

    hep->dogs = NULL;
    hep->dog_c_shape = malloc(sizeof(*hep->dog_c_shape));
    hep->dog_c_shape->centre.x = 0;
    hep->dog_c_shape->centre.y = 0;
    hep->dog_c_shape->radius =0;
    hep->dog_c_shape->w = 40;
    hep->dog_c_shape->h = 40;
    hep->dog_c_shape->type = JTY_RECT;

    hep->dog_c_shapes = malloc(4*sizeof(*hep->dog_c_shapes));
    for (i = 0; i < 4; i++) {
        hep->dog_c_shapes[i] = hep->dog_c_shape;
    }

    return hep;
}

void free_herdem_eng(herdem_eng *hep)
{
    free(hep->sheep_c_shape);
    free(hep->sheep_c_shapes);
    jty_eng_free();
}

typedef struct herdem_sheep {
    jty_actor actor;
    double max_speed;
    double normal_speed;
} herdem_sheep;


int is_in_scare_window(jty_vector r_rel)
{
    /* b = (cos(22.5))**2 */
    double b = 0.8535533905932737;
    float x;

    x = 1 / (1 + r_rel.y * r_rel.y / (r_rel.x * r_rel.x));

    return x > b && r_rel.x > 0;
}

void sheep_iterator(jty_actor *actor)
{
    herdem_sheep *sheep = (herdem_sheep *)actor;
    jty_actor_ls *dog_ls;
    herdem_dog *dog;
    jty_vector r_rel;
    double flee_speed;
    double r_rel_mag_sq; /* Magnitude squared of r_rel */
    bool panicked = false;
   
    for (dog_ls = herdem_engine->dogs; dog_ls != NULL; dog_ls = dog_ls->next) {
        dog = (herdem_dog *)(dog_ls->actor);
        r_rel.x = sheep->actor.x - dog->actor.x;
        r_rel.y = sheep->actor.y - dog->actor.y;
        r_rel_mag_sq = jty_vector_mag_sq(r_rel); 
        if (r_rel_mag_sq < pow(dog->scare_radius_max, 2)) {
            if (is_in_scare_window(r_rel)) {
                double r_rel_mag = sqrt(r_rel_mag_sq);
                flee_speed = sheep->max_speed / (dog->scare_radius_min \
                        - dog->scare_radius_max) * r_rel_mag \
                             - sheep->max_speed * dog->scare_radius_max/ \
                             (dog->scare_radius_min - dog->scare_radius_max) \
                             + sheep->normal_speed;
                sheep->actor.vx = 1 / r_rel_mag * r_rel.x * flee_speed;
                sheep->actor.vy = 1 / r_rel_mag * r_rel.y * flee_speed;
                panicked = true;
            }
        }
    }

    if (!panicked) {
        jty_vector v = {.x = sheep->actor.vx, .y = sheep->actor.vy};
        double sheep_speed = sqrt(jty_vector_mag_sq(v));
        sheep->actor.vx = 1 / sheep_speed * sheep->actor.vx  * sheep->normal_speed;
        sheep->actor.vy = 1 / sheep_speed * sheep->actor.vy  * sheep->normal_speed;
    }


}

herdem_sheep *herdem_sheep_init(herdem_sheep *sheep)
{
    sheep = (herdem_sheep *)jty_actor_init(
            (jty_actor *)sheep,
            SHEEP,
            16,
            124, 124, "images/sprites/sheep/NW_walking.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/N_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/NE_walking.png", herdem_engine->sheep_c_shapes,
            106, 69, "images/sprites/sheep/E_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SE_walking.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/S_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SW_walking.png", herdem_engine->sheep_c_shapes,
            106, 69, "images/sprites/sheep/W_walking.png", herdem_engine->sheep_c_shapes,

            124, 124, "images/sprites/sheep/NW_still.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/N_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/NE_still.png", herdem_engine->sheep_c_shapes,
            106, 69, "images/sprites/sheep/E_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SE_still.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/S_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SW_still.png", herdem_engine->sheep_c_shapes,
            106, 69, "images/sprites/sheep/W_still.png", herdem_engine->sheep_c_shapes
            );

    sheep->max_speed = 2.5;
    sheep->normal_speed = 1.2;
    jty_actor_add_i_handler((jty_actor *)sheep, animation_handler);
    jty_actor_add_i_handler((jty_actor *)sheep, sheep_iterator);
    jty_actor_add_m_handler((jty_actor *)sheep, sheep_wall_handler, "b");

    herdem_engine->sheeps = jty_actor_ls_add(herdem_engine->sheeps, (jty_actor *)sheep);

    return sheep;
}

herdem_sheep *new_herdem_sheep()
{
    herdem_sheep *sheep;

    sheep = malloc(sizeof(*sheep));
    return herdem_sheep_init(sheep);
}

void free_herdem_sheep(herdem_sheep *sheep)
{
    herdem_engine->sheeps = jty_actor_ls_rm(herdem_engine->sheeps, (jty_actor *)sheep);
    jty_eng_free_actor((jty_actor *)sheep);
}

herdem_dog *herdem_dog_init(herdem_dog *dog)
{
    dog = (herdem_dog *)jty_actor_init(
            (jty_actor *)dog,
            DOGS,
            16,
            127, 127, "images/sprites/dog/NW_walking.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/N_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/NE_walking.png", herdem_engine->dog_c_shapes,
            108, 71, "images/sprites/dog/E_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SE_walking.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/S_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SW_walking.png", herdem_engine->dog_c_shapes,
            108, 71, "images/sprites/dog/W_walking.png", herdem_engine->dog_c_shapes,

            127, 127, "images/sprites/dog/NW_walking.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/N_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/NE_still.png", herdem_engine->dog_c_shapes,
            108, 71, "images/sprites/dog/E_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SE_still.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/S_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SW_still.png", herdem_engine->dog_c_shapes,
            108, 71, "images/sprites/dog/W_still.png", herdem_engine->dog_c_shapes
            );

    dog->normal_speed = 2;
    dog->player_actions = 0;
    dog->scare_radius_min = 60;
    dog->scare_radius_max = 160;
    jty_actor_add_i_handler((jty_actor *)dog, input_handler);
    jty_actor_add_i_handler((jty_actor *)dog, animation_handler);
    jty_actor_add_m_handler((jty_actor *)dog, dog_wall_handler, "b");

    herdem_engine->dogs = jty_actor_ls_add(herdem_engine->dogs, (jty_actor *)dog);

    return dog;
}

herdem_dog *new_herdem_dog()
{
    herdem_dog *dog;

    dog = malloc(sizeof(*dog));

    return herdem_dog_init(dog);
}

void free_herdem_dog(herdem_dog *dog)
{
    jty_eng_free_actor((jty_actor *)dog);
}

int main(void)
{
    int map_w = 25, map_h = 18, tw = 32, th = 32;
    herdem_dog *dog;
    herdem_sheep *sheep;
    SDL_Event selection;
    int carry_on = 1;
    Uint32 start_t, curr_t;
    Uint32 c_frame = 0, p_frame = 0;
    int ef = 0;

    herdem_engine = new_herdem_eng();
    
    /* Creating map */
    if(!(herdem_engine->main_engine.map = jty_new_map(
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

    dog = new_herdem_dog();
    dog->actor.x = dog->actor.px = 400;
    dog->actor.y = dog->actor.py = 300;
    dog->actor.vx = dog->actor.vy = 0.0;

    sheep = new_herdem_sheep();
    sheep->actor.x = sheep->actor.px = 100;
    sheep->actor.y = sheep->actor.py = 100;
    sheep->actor.vx = 1;
    sheep->actor.vy = 0;

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
        herdem_engine->main_engine.elapsed_frames = ((double)(curr_t \
                    - start_t) / 1000. * FPS); 
        ef = (int)(herdem_engine->main_engine.elapsed_frames - c_frame);

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

    free_herdem_eng(herdem_engine);

    return 0;
}
