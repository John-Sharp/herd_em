#include "jaunty/jaunty.h"
#include "herdem_timer.h"
#include "herdem.h"

/**
 * Allocates and initialises a new timer
 */
jty_txt_actor *new_herdem_timer(int x, int y, jty_map *map)
{
    jty_txt_actor *timer = malloc(sizeof(*timer));

    herdem_timer_init(timer, x, y, map);

    return timer;
}

/**
 * Initialises a herdem_timer. Automatically called 
 * by `new_herdem_timer`
 */
jty_txt_actor *herdem_timer_init(
        jty_txt_actor *timer,
        int x,
        int y,
        jty_map *map)
{
    jty_txt_actor_init(timer, 1, 
            map,
            map->map_rect.w,
            20);
    jty_txt_actor_set_text(
            timer,
            "<span foreground=\"#FFFFFF\" >00:00</span>");

    timer->parent.x = timer->parent.px = x;
    timer->parent.y = timer->parent.py = y;
    jty_actor_add_i_handler((jty_actor *)timer, herdem_timer_update);

    return timer;
}

/**
 * Frees resources of the `timer`
 */
//jty_txt_actor *free_herdem_timer(jty_txt_actor *timer);

/**
 * Iteration handler, updates the timer with the correct
 * time
 */
void herdem_timer_update(jty_actor *actor)
{
    jty_txt_actor *timer = (jty_txt_actor *)actor;

    int minutes_elapsed = herdem_engine->level_time / 1000. / 60.;
    int seconds_elapsed = (int)(herdem_engine->level_time / 1000.)  % 60;
    char timer_text[200];

    sprintf(
            timer_text,
            "<span foreground=\"#FFFFFF\" >%i:%02i</span>",
            minutes_elapsed,
            seconds_elapsed);

    jty_txt_actor_set_text(timer, timer_text);

    return;
}
