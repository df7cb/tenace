#include "bridge.h"

struct win_t {
	GtkWindow *win;
	int n_boards;
	int cur;
	board *boards;
};

extern board *b;

void show_board (board *b);
void button_set_markup(card c, char *text);
void button_clear_markups();
void create_card_buttons ();

