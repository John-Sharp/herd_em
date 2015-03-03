#ifndef HERDEM_SHEEP_H
#define HERDEM_SHEEP_H

#include "jaunty/jaunty.h"

typedef enum herdem_sheep_consts {
    HERDEM_SHEEP_C_SHAPE_W = 40,
    HERDEM_SHEEP_C_SHAPE_H = 40,
    HERDEM_SHEEP_NUM_FRAMES = 4,
    HERDEM_SHEEP_GROUP_NUM = 1<<1,
    /**
     * Number of frames after first hitting a wall
     * that sheep is considered to be finished hitting
     * the wall
     */
    HERDEM_SHEEP_WALL_RECOVER_FRAMES = 10
} herdem_sheep_consts;

typedef struct herdem_sheep herdem_sheep;

struct herdem_sheep {
    jty_actor parent;
    double normal_speed;
    double fear_acceleration;
    double mi; /* inverse mass */
    bool panicked;
    int touching_wall;
};

/**
 * Allocates and initialises a new
 * herdem_sheep
 */
herdem_sheep *new_herdem_sheep();

/**
 * Initialises a herdem_sheep. Automatically called
 * by new_herdem_sheep
 */
herdem_sheep *herdem_sheep_init(herdem_sheep *sheep);

/**
 * Frees resources of the `sheep'
 */
void free_herdem_sheep(herdem_sheep *sheep);

/**
 * Handler for when two sheep collide
 */
void herdem_sheep_sheep_collision_handler(jty_c_info *c_info);

/**
 * Handler for when a sheep hits the wall
 */
void herdem_sheep_wall_handler(jty_c_info *c_info);

/**
 *
 */
void herdem_sheep_iterator(jty_actor *actor);

#endif
