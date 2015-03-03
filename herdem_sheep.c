#include "herdem.h"
#include "herdem_sheep.h"
#include "herdem_dog.h"

/**
 * Allocates and initialises a new
 * herdem_sheep
 */
herdem_sheep *new_herdem_sheep()
{
    herdem_sheep *sheep;

    sheep = malloc(sizeof(*sheep));
    return herdem_sheep_init(sheep);
}

/**
 *
 * Initialises a herdem_sheep. Automatically called
 * by new_herdem_sheep
 */
herdem_sheep *herdem_sheep_init(herdem_sheep *sheep)
{
    sheep = (herdem_sheep *)jty_actor_init(
            (jty_actor *)sheep,
            HERDEM_SHEEP_GROUP_NUM,
            jty_engine->map,
            16,
            106, 69, "images/sprites/sheep/E_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/NE_walking.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/N_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/NW_walking.png", herdem_engine->sheep_c_shapes,
            106, 69, "images/sprites/sheep/W_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SW_walking.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/S_walking.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SE_walking.png", herdem_engine->sheep_c_shapes,

            106, 69, "images/sprites/sheep/E_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/NE_still.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/N_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/NW_still.png", herdem_engine->sheep_c_shapes,
            106, 69, "images/sprites/sheep/W_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SW_still.png", herdem_engine->sheep_c_shapes,
            69, 106, "images/sprites/sheep/S_still.png", herdem_engine->sheep_c_shapes,
            124, 124, "images/sprites/sheep/SE_still.png", herdem_engine->sheep_c_shapes
            );

    sheep->normal_speed = 0.9;
    sheep->mi = 1;
    sheep->fear_acceleration = 5;
    sheep->panicked = false;

    jty_actor_add_i_handler((jty_actor *)sheep, herdem_animation_handler);
    jty_actor_add_i_handler((jty_actor *)sheep, herdem_sheep_iterator);
    jty_actor_add_m_handler((jty_actor *)sheep, herdem_sheep_wall_handler, "b");

    herdem_engine->sheeps = jty_actor_ls_add(herdem_engine->sheeps, (jty_actor *)sheep);

    return sheep;
}

/**
 * Frees resources of the `sheep'
 */
void free_herdem_sheep(herdem_sheep *sheep)
{
    herdem_engine->sheeps = jty_actor_ls_rm(herdem_engine->sheeps, (jty_actor *)sheep);
    free_jty_actor((jty_actor *)sheep);
}

/**
 * Handler for when two sheep collide
 */
void herdem_sheep_sheep_collision_handler(jty_c_info *c_info)
{
    herdem_sheep *sheep1 = (herdem_sheep *)c_info->e1.actor;
    herdem_sheep *sheep2 = (herdem_sheep *)c_info->e2.actor;
    double c = 1; /* coefficient of restitution */
    double rmi1 = sheep1->mi / (sheep1->mi + sheep2->mi); /* Reduced inverse mass
                                                            of sheep 1 */
    double rmi2 = sheep2->mi / (sheep1->mi + sheep2->mi); /* Reduced inverse mass
                                                            of sheep 2 */

    jty_vector v_rel = {
        .x = sheep1->parent.vx - sheep2->parent.vx,
        .y = sheep1->parent.vy - sheep2->parent.vy
    };

    if (c_info->normal.x) {
        sheep1->parent.vx -= (1 + c) * v_rel.x * rmi1;
        sheep2->parent.vx += (1 + c) * v_rel.x * rmi2;
    } else {
        sheep1->parent.vy -= (1 + c) * v_rel.y * rmi1;
        sheep2->parent.vy += (1 + c) * v_rel.y * rmi2;
    }

    sheep2->parent.x += rmi2 * c_info->normal.x * (c_info->penetration + 1);
    sheep2->parent.y += rmi2 * c_info->normal.y * (c_info->penetration + 1);

    sheep1->parent.x -= rmi1 * c_info->normal.x * (c_info->penetration + 1);
    sheep1->parent.y -= rmi1 * c_info->normal.y * (c_info->penetration + 1);
}

/**
 * Handler for when a sheep hits the wall
 */
void herdem_sheep_wall_handler(jty_c_info *c_info)
{
    jty_actor *a = c_info->e1.actor;
    herdem_sheep *sheep = (herdem_sheep *)a;

    a->x -= c_info->normal.x * (c_info->penetration + 1);
    a->y -= c_info->normal.y * (c_info->penetration + 1);
    sheep->touching_wall = HERDEM_SHEEP_WALL_RECOVER_FRAMES;

    if (c_info->normal.x)
        a->vx *= -1;
    if (c_info->normal.y)
        a->vy *= -1;

    if (sheep->panicked == true) {
        if (c_info->normal.x) {
            a->vx = 0;
        }else {
            a->vy = 0;
        }
    }
}

/**
 * Gets called for each sheep each frame. Repels sheep
 * from dogs and keeps track of when sheep last hit a wall
 */
void herdem_sheep_iterator(jty_actor *actor)
{
    herdem_sheep *sheep = (herdem_sheep *)actor;
    jty_actor_ls *dog_ls;
    herdem_dog *dog;
    jty_vector r_rel;
    double r_rel_mag_sq; /* Magnitude squared of r_rel */

    sheep->parent.ax = 0;
    sheep->parent.ay = 0;
    sheep->panicked = false;

    if (jty_actor_has_left_map(actor)) {
        herdem_engine->saved_sheeps += 1;
        
        fprintf(stderr, "A sheep has been saved!!\n");
        free_herdem_sheep(sheep);
        return;
    }
   
    for (dog_ls = herdem_engine->dogs; dog_ls != NULL; dog_ls = dog_ls->next) {
        dog = (herdem_dog *)(dog_ls->actor);
        r_rel.x = sheep->parent.x - dog->actor.x;
        r_rel.y = sheep->parent.y - dog->actor.y;
        r_rel_mag_sq = jty_vector_mag_sq(r_rel); 
        if (r_rel_mag_sq < pow(dog->scare_radius_max, 2)) {
            sheep->parent.ax += 1 / r_rel_mag_sq * r_rel.x * sheep->fear_acceleration;
            sheep->parent.ay += 1 / r_rel_mag_sq * r_rel.y * sheep->fear_acceleration;
            sheep->panicked = true;
        }
    }

    jty_vector v = {.x = sheep->parent.vx, .y = sheep->parent.vy};
    double sheep_speed = sqrt(jty_vector_mag_sq(v));
    if (sheep_speed == 0) {
        eight_way_direction_change((jty_actor *)sheep);
        return;
    }

    sheep->parent.ax += -1 / sheep_speed * v.x * (sheep_speed - sheep->normal_speed) * 1;
    sheep->parent.ay += -1 / sheep_speed * v.y * (sheep_speed - sheep->normal_speed) * 1;

    if (
            sheep->touching_wall < 0 ||
            sheep->touching_wall == HERDEM_SHEEP_WALL_RECOVER_FRAMES)
    {
        eight_way_direction_change((jty_actor *)sheep);
    }

    sheep->touching_wall -= 1;
}
