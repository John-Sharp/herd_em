#ifndef HERDEM_TIMER_H
#define HERDEM_TIMER

/**
 * Allocates and initialises a new timer
 */
jty_txt_actor *new_herdem_timer(int x, int y);

/**
 * Initialises a herdem_timer. Automatically called 
 * by `new_herdem_timer`
 */
jty_txt_actor *herdem_timer_init(jty_txt_actor *timer, int x, int y);

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
