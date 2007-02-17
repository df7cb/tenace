#include "bridge.h"

extern board *b;

void show_board (board *b);
void show_played_card(board *b, seat s, card c);
void label_set_markup(card c, char *text);
void label_clear_markups();
void create_card_labels ();

