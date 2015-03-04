#ifndef HERDEM_INFO_BOARD_H
#define HERDEM_INFO_BOARD_H

#include "jaunty/jaunty.h"

typedef struct herdem_info_board herdem_info_board;

struct herdem_info_board {
    jty_map parent;
    jty_txt_actor *timer;
    jty_txt_actor *saved_tally;
};

/**
 * Allocates and initialises a new
 * info_board
 */
herdem_info_board *new_herdem_info_board();

/**
 * Initialises a info_board. Automatically called
 * by new_herdem_info_board
 */
herdem_info_board *herdem_info_board_init(herdem_info_board *info_board);

/**
 * Frees resources of the `info_board'
 */
void free_herdem_info_board(herdem_info_board *info_board);

#endif
