#ifndef HERDEM_SAVED_TALLY_H
#define HERDEM_SAVED_TALLY_H

#include "jaunty/jaunty.h"

/**
 * Allocates and initialises a new saved sheep tally
 */
jty_txt_actor *new_herdem_saved_tally(int x, int y, jty_map *map);

/**
 * Initialises a saved sheep tally. Automatically called 
 * by `new_herdem_saved_tally`
 */
jty_txt_actor *herdem_saved_tally_init(jty_txt_actor *saved_tally, int x, int y, jty_map *map);

/**
 * Frees resources of the `saved_tally`
 */
jty_txt_actor *free_herdem_saved_tally(jty_txt_actor *saved_tally);

/**
 * Iteration handler, updates the tally with the correct
 * score
 */
void herdem_saved_tally_update(jty_txt_actor *saved_tally);

#endif
