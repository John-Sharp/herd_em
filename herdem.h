#ifndef HERDEM_H
#define HERDEM_H


#include "jaunty/jaunty.h"
void eight_way_direction_change(jty_actor *a);
#include "herdem_sheep.h"

typedef enum herdem_consts {
    WIN_W = 800,
    WIN_H = 600,
    HERDEM_ANIMATION_FRAMERATE = 8,
    FPS = 300
} herdem_consts;


typedef enum herdem_direction{
    DIRECTION_E, DIRECTION_NE, DIRECTION_N,
    DIRECTION_NW, DIRECTION_W, DIRECTION_SW,
    DIRECTION_S, DIRECTION_SE 
} herdem_direction;

typedef enum herdem_player_action {
    HERDEM_MOVE_N = 1,
    HERDEM_MOVE_E = 1<<1,
    HERDEM_MOVE_S = 1<<2,
    HERDEM_MOVE_W = 1<<3} herdem_player_action;

#include "herdem_dog.h"

typedef struct herdem_eng herdem_eng;

struct herdem_eng {
    jty_eng parent;

    jty_map *info_board; /* Map for showing the current number of sheep saved,
                             time and other information */

    int total_sheeps;
    int target_sheeps;
    int saved_sheeps;

    Uint32 level_start_time; /* time in SDL time, in milliseconds, that
                                the level started */
    Uint32 level_time; /* length of time (in milliseconds) that the
                          player has been on the level */

    jty_actor_ls *sheeps;
    jty_shape *sheep_c_shape;
    jty_shape **sheep_c_shapes;

    jty_actor_ls *dogs;
    jty_shape *dog_c_shape;
    jty_shape **dog_c_shapes;
};

herdem_eng *herdem_engine;

/**
 * Creates a new herdem_eng
 */
herdem_eng *new_herdem_eng();

/**
 * Initialises a new herdem_eng, called automatically by
 * `new_herdem_eng`
 */
herdem_eng *herdem_eng_init(herdem_eng *eng);

/**
 * Frees resources of herdem_eng
 */
void free_herdem_eng(herdem_eng *hep);

/**
 * Iterates the herdem engine
 */
void herdem_iterate();

/**
 * Paints the maps and actors contained in the 
 * engine
 */
void herdem_paint();

/**
 * Handles the animation of a herdem actor
 */
void herdem_animation_handler(struct jty_actor *actor);

#endif
