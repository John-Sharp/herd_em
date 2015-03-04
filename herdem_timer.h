#ifndef HERDEM_TIMER_H
#define HERDEM_TIMER

#include "jaunty/jaunty.h"

/**
 * Allocates and initialises a new timer
 */
jty_txt_actor *new_herdem_timer(int x, int y, jty_map *map);

/**
 * Initialises a herdem_timer. Automatically called 
 * by `new_herdem_timer`
 */
jty_txt_actor *herdem_timer_init(jty_txt_actor *timer, int x, int y, jty_map *map);

/**
 * Frees resources of the `timer`
 */
jty_txt_actor *free_herdem_timer(jty_txt_actor *timer);

/**
 * Iteration handler, updates the timer with the correct
 * time
 */
void herdem_timer_update(jty_actor *actor);
#endif
