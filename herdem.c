#include "herdem.h"


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


/**
 * Creates a new herdem_eng
 */
herdem_eng *new_herdem_eng()
{
    herdem_eng *hep;

    hep = malloc(sizeof(*hep));

    return herdem_eng_init(hep);
}

/**
 * Initialises a new herdem_eng, called automatically by
 * `new_herdem_eng`
 */
herdem_eng *herdem_eng_init(herdem_eng *hep)
{
    int i;
    jty_eng_init((jty_eng *)hep, WIN_W, WIN_H);

    hep->sheeps = NULL;
    hep->sheep_c_shape = malloc(sizeof(*hep->sheep_c_shape));
    hep->sheep_c_shape->centre.x = 0;
    hep->sheep_c_shape->centre.y = 0;
    hep->sheep_c_shape->radius =0;
    hep->sheep_c_shape->w = HERDEM_SHEEP_C_SHAPE_W;
    hep->sheep_c_shape->h = HERDEM_SHEEP_C_SHAPE_H;
    hep->sheep_c_shape->type = JTY_RECT;

    hep->sheep_c_shapes = malloc(
            HERDEM_SHEEP_NUM_FRAMES*sizeof(*hep->sheep_c_shapes));
    for (i = 0; i < HERDEM_SHEEP_NUM_FRAMES; i++) {
        hep->sheep_c_shapes[i] = hep->sheep_c_shape;
    }

    hep->dogs = NULL;
    hep->dog_c_shape = malloc(sizeof(*hep->dog_c_shape));
    hep->dog_c_shape->centre.x = 0;
    hep->dog_c_shape->centre.y = 0;
    hep->dog_c_shape->radius =0;
    hep->dog_c_shape->w = HERDEM_DOG_C_SHAPE_W;
    hep->dog_c_shape->h = HERDEM_DOG_C_SHAPE_H;
    hep->dog_c_shape->type = JTY_RECT;

    hep->dog_c_shapes = malloc(
            HERDEM_DOG_NUM_FRAMES*sizeof(*hep->dog_c_shapes));
    for (i = 0; i < HERDEM_DOG_NUM_FRAMES; i++) {
        hep->dog_c_shapes[i] = hep->dog_c_shape;
    }

    return hep;
}

herdem_eng *herdem_eng_clean_up_level(herdem_eng *eng)
{
    while(eng->dogs) {
        free_herdem_dog((herdem_dog *)(eng->dogs->actor));
    }

    while(eng->sheeps) {
        free_herdem_sheep((herdem_sheep *)(eng->sheeps->actor));
    }

    free_jty_map(jty_engine->map);
    jty_engine->map = NULL;

    return eng;
}

/**
 * Frees resources of herdem_eng
 */
void free_herdem_eng(herdem_eng *hep)
{
    free(hep->sheep_c_shape);
    free(hep->sheep_c_shapes);
    free(hep->dog_c_shape);
    free(hep->dog_c_shapes);
    jty_eng_free();
}

/**
 * Iterates the herdem engine
 */
void herdem_iterate()
{
    jty_map_iterate((jty_map *)herdem_engine->info_board);

    jty_iterate();
}

/**
 * Paints the maps and actors contained in the 
 * engine
 */
void herdem_paint()
{
    jty_map_paint((jty_map *)herdem_engine->info_board);

    jty_paint();
}

/**
 * Handles the animation of a herdem actor
 */
void herdem_animation_handler(struct jty_actor *actor)
{
    jty_sprite *sprite = actor->sprites[actor->current_sprite];
    int animation_frames_passed = jty_engine->elapsed_frames \
                                  / FPS * HERDEM_ANIMATION_FRAMERATE;

    actor->current_frame = animation_frames_passed % sprite->num_of_frames;
    return;
}
