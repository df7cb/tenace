#include "bridge.h"

struct win_t {
	GtkWidget *window;

	GtkWidget *card_button[52];
	GtkWidget *card_button_child[52];
	GtkWidget *card_button_container[52]; /* non-NULL if button is shown */

	int n_boards;
	int cur;
	board *boards[];
};

extern board *b;
extern struct win_t *win;

void show_board (board *b);
void button_set_markup(card c, char *text);
void button_clear_markups();
void create_card_buttons ();

