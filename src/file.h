#include "window_board.h"

int board_parse_line(const char *line, board *b, char handsep, char suitsep);
int board_load (window_board_t *win1, char *fname);
int board_load_popup (window_board_t *win, int append, char *filename);
int board_load_dialog (window_board_t *win, int append);

GString *board_format_line(board *b, char handsep, char suitsep);
int board_save (window_board_t *win, char *filename);
int board_save_dialog (window_board_t *win, int save_as);

