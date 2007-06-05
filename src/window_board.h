#ifndef WINDOW_CARD_H
#define WINDOW_CARD_H

#include "../handdisplay/hand_display.h"

#include "bridge.h"

/* defines */

#define CUR_BOARD (win->boards[win->cur])

/* types */

typedef struct _windown_board_t {
	GtkWidget *window;
	GtkStatusbar *statusbar;
	GtkMenu *board_menu;

	int n_boards;
	int n_boards_alloc;
	int cur;
	board **boards;

	HandDisplay *handdisp[4];
} window_board_t;

typedef enum _redraw_t {
	REDRAW_TITLE	= 1 << 0,
	REDRAW_NAMES	= 1 << 1,
	REDRAW_CONTRACT	= 1 << 2,
	REDRAW_TRICKS	= 1 << 3,
	REDRAW_HANDS	= 1 << 4,
	REDRAW_DD	= 1 << 5,
	REDRAW_PLAY	= 1 << 6,
	REDRAW_BIDDING	= 1 << 7,
	REDRAW_FULL	= -1
} redraw_t;

/* global variable */

extern window_board_t *win;

/* interface */

void show_board (board *b, redraw_t redraw);

void board_window_append_board (board *b);
void board_window_init ();
void board_statusbar(char *text);

void board_set_dealer (seat dealer);
void board_set_trumps (suit trumps);
void board_set_level (int level);
void board_set_vuln (int ns, int ew);
void board_toggle_doubled (int button); /* only useful for calls from GUI */

#endif /* WINDOW_CARD_H */
