#include "jaunty/jaunty.h"
#include "herdem_sheep.h"
#include "herdem_dog.h"

/**
 * Allocates and initialises new herdem_dog structure
 */
herdem_dog *new_herdem_dog()
{
    herdem_dog *dog;

    dog = malloc(sizeof(*dog));

    return herdem_dog_init(dog);
}

/**
 * Initialises new herdem_dog structure. Called by 
 * `new_hedem_dog` automatically
 */
herdem_dog *herdem_dog_init(herdem_dog *dog)
{
    dog = (herdem_dog *)jty_actor_init(
            (jty_actor *)dog,
            HERDEM_DOG_GROUP_NUM,
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
    jty_actor_add_i_handler((jty_actor *)dog, herdem_dog_input_handler);
    jty_actor_add_i_handler((jty_actor *)dog, herdem_animation_handler);
    jty_actor_add_m_handler((jty_actor *)dog, herdem_dog_wall_handler, "bc");

    herdem_engine->dogs = jty_actor_ls_add(herdem_engine->dogs, (jty_actor *)dog);

    return dog;
}

/**
 * Frees resources allocated to `dog`
 */
void free_herdem_dog(herdem_dog *dog)
{
    free_jty_actor((jty_actor *)dog);
}

/**
 * Handler for when dog collides with sheep
 */
void herdem_dog_sheep_collision_handler(jty_c_info *c_info)
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

/**
 * Handler for when dog hits the wall
 */
void herdem_dog_wall_handler(jty_c_info *c_info)
{
    jty_actor *a = c_info->e1.actor;

    a->x -= c_info->normal.x * (c_info->penetration + 1);
    a->y -= c_info->normal.y * (c_info->penetration + 1);
}

/**
 * Polls SDL for what keys are being pressed and stores
 * these on the dog
 */
void herdem_dog_input_handler(struct jty_actor *actor)
{
    SDL_Event selection;
    struct herdem_dog *dog = (herdem_dog *)actor;

    if(SDL_PeepEvents(&selection, 1,
                SDL_GETEVENT, SDL_EVENTMASK(SDL_KEYDOWN) |
                              SDL_EVENTMASK(SDL_KEYUP))){
        if (
                dog->player_actions == HERDEM_NONE &&
                selection.key.type == SDL_KEYUP
           ) {
            return;
        }


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

/**
 * Calculates the acceleration of the dog, based on keys have
 * been pressed
 */
void herdem_dog_calculate_acceleration(herdem_dog *dog)
{
    /* 1/sqrt(2) */
    double s2r = 0.7071067811865475;

    dog->parent.ax = 0;
    dog->parent.ay = 0;

    if (dog->player_actions & HERDEM_MOVE_N) {
        if (dog->player_actions & HERDEM_MOVE_S) {
            dog->parent.ay = 0;
        }
        dog->parent.ay = -dog->acceleration;
    } else if (dog->player_actions & HERDEM_MOVE_S) {
        dog->parent.ay = dog->acceleration;
    }

    if (dog->player_actions & HERDEM_MOVE_E) {
        if (dog->player_actions & HERDEM_MOVE_W) {
            dog->parent.ax = 0;
        }
        if (dog->parent.ay != 0) {
            dog->parent.ax = s2r * dog->acceleration;
            dog->parent.ay *= s2r;
        } else {
            dog->parent.ax = dog->acceleration;
        }
    } else if (dog->player_actions & HERDEM_MOVE_W) {
        if (dog->parent.ay != 0) {
            dog->parent.ax = -s2r * dog->acceleration;
            dog->parent.ay *= s2r;
        } else {
            dog->parent.ax = -dog->acceleration;
        }
    }
    jty_vector v = {.x = dog->parent.vx, .y = dog->parent.vy};
    double dog_speed = sqrt(jty_vector_mag_sq(v));
    if (dog_speed < 0.1 && dog->parent.ax == 0 && dog->parent.ay == 0) {
        dog->parent.vx = 0;
        dog->parent.vy = 0;
        eight_way_direction_change((jty_actor *)dog);
        return;
    }

    dog->parent.ax += -1 * dog->drag * v.x;
    dog->parent.ay += -1 * dog->drag * v.y;

    eight_way_direction_change((jty_actor *)dog);
}

// CURRENTLY THESE ARENT USED
herdem_direction herdem_dog_get_direction(herdem_dog *dog)
{
    return dog->parent.current_sprite % 8;
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
