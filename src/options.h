#include "window_board.h"

/* hooks into options window */
void window_options_board_populate (void);
void window_options_board_list_populate (void);

/* rcfile handling */
int read_config (window_board_t *win);
int write_config (window_board_t *win);
