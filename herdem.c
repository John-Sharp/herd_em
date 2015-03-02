#include "herdem.h"

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
    jty_map_iterate(herdem_engine->info_board);

    jty_iterate();
}

/**
 * Paints the maps and actors contained in the 
 * engine
 */
void herdem_paint()
{
    jty_map_paint(herdem_engine->info_board);

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
