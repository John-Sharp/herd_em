#ifndef HERDEM_DOG_H
#define HERDEM_DOG_H

#include "jaunty/jaunty.h"
#include "herdem_sheep.h"
#include "herdem.h"

typedef enum herdem_dog_consts {
    HERDEM_DOG_C_SHAPE_W = 40,
    HERDEM_DOG_C_SHAPE_H = 40,
    HERDEM_DOG_NUM_FRAMES = 4,
    HERDEM_DOG_GROUP_NUM = 1
} herdem_dog_consts;

typedef struct herdem_dog herdem_dog;

struct herdem_dog {
    jty_actor parent;
    double acceleration;
    double scare_radius_min; /* radius where sheep run away with their max. speed */
    double scare_radius_max; /* maximum radius where sheep can see dog */
    double drag;
    herdem_player_action player_actions;
};

/**
 * Allocates and initialises new herdem_dog structure
 */
herdem_dog *new_herdem_dog();

/**
 * Initialises new herdem_dog structure. Called by 
 * `new_hedem_dog` automatically
 */
herdem_dog *herdem_dog_init(herdem_dog *dog);

/**
 * Frees resources allocated to `dog`
 */
void free_herdem_dog(herdem_dog *dog);

/**
 * Handler for when dog collides with sheep
 */
void herdem_dog_sheep_collision_handler(jty_c_info *c_info);

/**
 * Handler for when dog hits the wall
 */
void herdem_dog_wall_handler(jty_c_info *c_info);

/**
 * Polls SDL for what keys are being pressed and stores
 * these on the dog
 */
void herdem_dog_input_handler(struct jty_actor *actor);

/**
 * Calculates the acceleration of the dog, based on keys have
 * been pressed
 */
void herdem_dog_calculate_acceleration(herdem_dog *dog);
#endif
