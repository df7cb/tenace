#ifndef WINDOW_CARD_H
#define WINDOW_CARD_H

#include "../handdisplay/hand_display.h"

#include "bridge.h"

typedef struct _windown_board_t {
	GtkWidget *window;
	GtkStatusbar *statusbar;

	int n_boards;
	int cur;
	board *boards[100]; // FIXME

	HandDisplay *handdisp[4];
} window_board_t;

extern board *b;
extern window_board_t *win;

void show_board (board *b);
void button_set_markup(card c, char *text);
void button_clear_markups();

void board_window_init ();
void board_statusbar(char *text);

#endif /* WINDOW_CARD_H */
