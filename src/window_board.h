#ifndef WINDOW_CARD_H
#define WINDOW_CARD_H

#include "../handdisplay/hand_display.h"

#include "bridge.h"

/* defines */

#define CUR_BOARD (win->boards[win->cur])

/* make sure some code is not called twice */
#define PROTECT_BEGIN do { if (protect++) return; } while (0)
#define PROTECT_END do { protect = 0; } while (0)
#define PROTECT_RETURN do { protect = 0; return; } while (0)

/* types */

typedef struct _window_board_t {
	/* GTK */
	GtkWidget *window;
	GtkStatusbar *statusbar;
	GtkWidget *board_menu;
	HandDisplay *handdisp[4];
	HandDisplay *table;

	/* bidding display */
	GtkTreeView *bidding;
	GtkListStore *bidding_store;
	GtkTreeViewColumn *bidding_column[4];
	GtkWidget *bidding_header[4];

	/* config */
	int show_played_cards;
	seat show_hands;
	seat show_dd_scores;
	seat autoplay;
	int card_width;

	/* boards */
	GString *filename;
	GString *title;
	int n_boards;
	int n_boards_alloc;
	int cur;
	board **boards;

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
	REDRAW_BOARD_LIST = 1 << 8,
	REDRAW_BOARD	= 0xff, /* everything except the menu */
	REDRAW_FULL	= -1
} redraw_t;

/* global variable */

extern window_board_t *win;
extern int protect; /* inhibit infinite loops in handlers */

/* interface */

void show_board (board *b, redraw_t redraw);
void start_autoplay ();

int board_window_append_board (window_board_t *win, board *b); /* returns number of appended board */
void board_window_init ();
void board_window_set_style (window_board_t *win, int style);
void board_statusbar(char *text);

/* stubs for calls from gui */
void board_set_declarer (seat declarer);
void board_set_dealer (seat dealer);
void board_set_trumps (suit trumps);
void board_set_level (int level);
void board_set_vuln (int ns, int ew);
void board_set_doubled (int doubled);

/* rcfile handling */
int read_config (window_board_t *win);
int write_config (window_board_t *win);

#endif /* WINDOW_CARD_H */
