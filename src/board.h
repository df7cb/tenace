#include "bridge.h"

extern board *b;

void show_board (board *b);
void label_set_markup(card c, char *text);
void label_clear_markups();
void create_card_labels ();

void board_save(board *b);
int board_load(char *fname, board *b);
