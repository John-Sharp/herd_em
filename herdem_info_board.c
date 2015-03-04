#include "herdem_info_board.h"
#include "herdem.h"

/**
 * Allocates and initialises a new
 * info_board
 */
herdem_info_board *new_herdem_info_board()
{
    herdem_info_board *info_board = malloc(sizeof(*info_board));

    herdem_info_board_init(info_board);

    return info_board;
}

/**
 * Initialises an info_board. Automatically called
 * by new_herdem_info_board
 */
herdem_info_board *herdem_info_board_init(herdem_info_board *info_board)
{
    int ib_w = 1, ib_h = 1, ib_tw = WIN_W, ib_th = WIN_H;

    jty_map_init(
            (jty_map *)info_board,
            ib_w,
            ib_h,
            ib_tw,
            ib_th,
            "images/ib.png",
            "a",
            "a",
            "a");
    info_board->parent.map_rect.y = WIN_H - ib_th;

    info_board->timer = new_herdem_timer(
            ib_tw / 2,
            20, 
            (jty_map *)info_board);

    info_board->saved_tally = new_herdem_saved_tally(
             ib_tw / 2,
             20,
             (jty_map *)info_board);
    return info_board;
}
