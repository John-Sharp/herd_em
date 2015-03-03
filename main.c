#include "jaunty/jaunty.h"
#include "herdem.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

#define DEBUG_MODE


enum { DOGS = 1 };

void dog_wall_handler(jty_c_info *c_info)
{
    jty_actor *a = c_info->e1.actor;

    a->x -= c_info->normal.x * (c_info->penetration + 1);
    a->y -= c_info->normal.y * (c_info->penetration + 1);
}

void dog_sheep_collision_handler(jty_c_info *c_info)
{
    jty_actor *dog = c_info->e1.actor;
    jty_actor *sheep = c_info->e2.actor;

    jty_vector v_rel = {sheep->vx - dog->vx, sheep->vy - dog->vy};

    sheep->x += c_info->normal.x * (c_info->penetration + 1);
    sheep->y += c_info->normal.y * (c_info->penetration + 1);

    dog->x -= c_info->normal.x * (c_info->penetration + 1);
    dog->y -= c_info->normal.y * (c_info->penetration + 1);

    if (c_info->normal.x) {
        dog->vx += 2 * v_rel.x;
    } else {
        dog->vy += 2 * v_rel.y;
    }
}



void herdem_dog_calculate_acceleration(herdem_dog *dog)
{
    /* 1/sqrt(2) */
    double s2r = 0.7071067811865475;

    dog->actor.ax = 0;
    dog->actor.ay = 0;

    if (dog->player_actions & HERDEM_MOVE_N) {
        if (dog->player_actions & HERDEM_MOVE_S) {
            dog->actor.ay = 0;
        }
        dog->actor.ay = -dog->acceleration;
    } else if (dog->player_actions & HERDEM_MOVE_S) {
        dog->actor.ay = dog->acceleration;
    }

    if (dog->player_actions & HERDEM_MOVE_E) {
        if (dog->player_actions & HERDEM_MOVE_W) {
            dog->actor.ax = 0;
        }
        if (dog->actor.ay != 0) {
            dog->actor.ax = s2r * dog->acceleration;
            dog->actor.ay *= s2r;
        } else {
            dog->actor.ax = dog->acceleration;
        }
    } else if (dog->player_actions & HERDEM_MOVE_W) {
        if (dog->actor.ay != 0) {
            dog->actor.ax = -s2r * dog->acceleration;
            dog->actor.ay *= s2r;
        } else {
            dog->actor.ax = -dog->acceleration;
        }
    }
    jty_vector v = {.x = dog->actor.vx, .y = dog->actor.vy};
    double dog_speed = sqrt(jty_vector_mag_sq(v));
    if (dog_speed < 0.1 && dog->actor.ax == 0 && dog->actor.ay == 0) {
        dog->actor.vx = 0;
        dog->actor.vy = 0;
        eight_way_direction_change((jty_actor *)dog);
        return;
    }

    dog->actor.ax += -1 * dog->drag * v.x;
    dog->actor.ay += -1 * dog->drag * v.y;

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
    }
    herdem_dog_calculate_acceleration(dog);
}

herdem_direction herdem_dog_get_direction(herdem_dog *dog)
{
    return dog->actor.current_sprite % 8;
}

int herdem_dog_has_in_scare_window(herdem_dog *dog, jty_vector r_rel)
{
    /* b = (cos(22.5))**2 */
    double b = 0.8535533905932737;
    jty_vector r_rel_rot;
    double x;
    double theta = (double)herdem_dog_get_direction(dog) * M_PI/4;

    r_rel_rot.x = r_rel.x * cos(theta) - r_rel.y * sin(theta);
    r_rel_rot.y = r_rel.x * sin(theta) + r_rel.y * cos(theta);

    x = 1 / (1 + r_rel_rot.y * r_rel_rot.y / (r_rel_rot.x * r_rel_rot.x));

    return x > b && r_rel_rot.x > 0;
}

herdem_dog *herdem_dog_init(herdem_dog *dog)
{
    dog = (herdem_dog *)jty_actor_init(
            (jty_actor *)dog,
            DOGS,
            jty_engine->map,
            16,
            108, 71, "images/sprites/dog/E_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/NE_walking.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/N_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/NW_walking.png", herdem_engine->dog_c_shapes,
            108, 71, "images/sprites/dog/W_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SW_walking.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/S_walking.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SE_walking.png", herdem_engine->dog_c_shapes,

            108, 71, "images/sprites/dog/E_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/NE_still.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/N_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/NW_still.png", herdem_engine->dog_c_shapes,
            108, 71, "images/sprites/dog/W_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SW_still.png", herdem_engine->dog_c_shapes,
            71, 108, "images/sprites/dog/S_still.png", herdem_engine->dog_c_shapes,
            127, 127, "images/sprites/dog/SE_still.png", herdem_engine->dog_c_shapes
            );

    double max_speed = 2.2;
    double half_life = 10;

    dog->drag = 1 / half_life *  0.6931471805599453;
    dog->acceleration = max_speed * dog->drag;

    dog->player_actions = 0;
    dog->scare_radius_min = 60;
    dog->scare_radius_max = 160;
    jty_actor_add_i_handler((jty_actor *)dog, input_handler);
    jty_actor_add_i_handler((jty_actor *)dog, herdem_animation_handler);
    jty_actor_add_m_handler((jty_actor *)dog, dog_wall_handler, "bc");

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
    free_jty_actor((jty_actor *)dog);
}

void timer_update(jty_actor *actor)
{
    jty_txt_actor *timer = (jty_txt_actor *)actor;

    int minutes_elapsed = herdem_engine->level_time / 1000. / 60.;
    int seconds_elapsed = (int)(herdem_engine->level_time / 1000.)  % 60;
    char timer_text[200];
    sprintf(timer_text, "<span foreground=\"#FFFFFF\" >%i:%02i</span>", minutes_elapsed, seconds_elapsed);

    jty_txt_actor_set_text(timer, timer_text);

    return;
}

void set_up_level_one()
{
    int map_w = 25, map_h = 17, tw = 32, th = 32;
    int ib_w = 1, ib_h = 1, ib_tw = WIN_W, ib_th = WIN_H;
    herdem_dog *dog;
    herdem_sheep *sheep;
    jty_txt_actor *timer, *saved_tally;

    /* Creating info board */

    if(!(herdem_engine->info_board = new_jty_map(
                    ib_w, ib_h, ib_tw, ib_th,
                    "images/ib.png",
                    "a",
                    "a",
                    "a"))) {
        fprintf(stderr, "Error loading info board!\n");
        exit(1);
    }
    herdem_engine->info_board->map_rect.y = WIN_H - ib_th;

    timer = new_jty_txt_actor(
            1,
            800,
            20,
            herdem_engine->info_board);
    jty_txt_actor_set_text(timer, "<span foreground=\"#FFFFFF\" >00:00</span>");

    timer->parent.x = timer->parent.px = 400;
    timer->parent.y = timer->parent.py = 20;
    jty_actor_add_i_handler((jty_actor *)timer, timer_update);

    saved_tally = new_jty_txt_actor(
            1,
            800,
            20,
            herdem_engine->info_board);
    pango_layout_set_alignment(saved_tally->layout, PANGO_ALIGN_RIGHT);
    jty_txt_actor_set_text(saved_tally, "<span foreground=\"#FFFFFF\"> 0/2 sheep herded </span>");
    saved_tally->parent.x = saved_tally->parent.px = 400;
    saved_tally->parent.y = saved_tally->parent.py = 20;


    /* Creating map */
    if(!(herdem_engine->parent.map = new_jty_map(
                map_w, map_h, tw, th,
                "images/map.png",
                "abcdefg"
                "hijklmn"
                "opqrstu",
                "abbbbbbbbbbbbbbbbbbbbbbbc"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiij"
                "hiiiiiiiiiiiiiiiiiiiiiiir"
                "hiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiii"
                "hiiiiiiiiiiiiiiiiiiiiiiik"
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
                "baaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaac"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "baaaaaaaaaaaaaaaaaaaaaaab"
                "bbbbbbbbbbbbbbbbbbbbbbbbb"
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
            DOGS,
            HERDEM_SHEEP_GROUP_NUM,
            dog_sheep_collision_handler);

    dog = new_herdem_dog();
    dog->actor.x = dog->actor.px = 400;
    dog->actor.y = dog->actor.py = 300;
    dog->actor.vx = dog->actor.vy = 0.0;

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

    herdem_engine->target_sheeps = 2;
    herdem_engine->saved_sheeps = 0;
    herdem_engine->level_start_time = SDL_GetTicks();
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
