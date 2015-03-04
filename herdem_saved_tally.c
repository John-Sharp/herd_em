#include "jaunty/jaunty.h"
#include "herdem_saved_tally.h"
#include "herdem.h"

/**
 * Allocates and initialises a new saved sheep tally
 */
jty_txt_actor *new_herdem_saved_tally(int x, int y, jty_map *map)
{
    jty_txt_actor *tally = malloc(sizeof(*tally));

    return herdem_saved_tally_init(tally, x, y, map);
}

/**
 * Initialises a saved sheep tally. Automatically called 
 * by `new_herdem_saved_tally`
 */
jty_txt_actor *herdem_saved_tally_init(jty_txt_actor *saved_tally, int x, int y, jty_map *map)
{
    jty_txt_actor_init(
            saved_tally,
            1,
            map,
            map->map_rect.w,
            20);
    pango_layout_set_alignment(saved_tally->layout, PANGO_ALIGN_RIGHT);
    herdem_saved_tally_update(saved_tally);

    saved_tally->parent.x = saved_tally->parent.px = x;
    saved_tally->parent.y = saved_tally->parent.py = y;

    return saved_tally;
}

/**
 * Iteration handler, updates the tally with the correct
 * score
 */
void herdem_saved_tally_update(jty_txt_actor *saved_tally)
{
    char text[200];
    sprintf(
            text,
            "<span foreground=\"#FFFFFF\"> %d/%d sheep herded </span>",
            herdem_engine->saved_sheeps,
            herdem_engine->target_sheeps);

    jty_txt_actor_set_text(
            saved_tally,
            text);
}
