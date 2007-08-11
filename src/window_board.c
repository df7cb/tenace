/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2007 Christoph Berg <cb@df7cb.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "../handdisplay/hand_display.h"

#include "window_board.h"

#include "functions.h"
#include "interface.h"
#include "solve.h"
#include "support.h"
#include "window_line_entry.h"
#include "window_play.h"

window_board_t *win; // FIXME static?
static GString *rcfile = NULL;

static void
board_menu_select (GtkWidget *menuitem, int *n)
{
	win->cur = *n;
	show_board (win->boards[*n], REDRAW_BOARD);
}

static void
board_window_rebuild_board_menu (window_board_t *win)
{
	if (gtk_menu_item_get_submenu (GTK_MENU_ITEM (win->board_menu)))
		gtk_menu_item_remove_submenu (GTK_MENU_ITEM (win->board_menu));

	GtkWidget *submenu = gtk_menu_new ();
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (win->board_menu), submenu);

	int i;
	GSList *group = NULL;
	for (i = 0; i < win->n_boards; i++) {
		board *b = win->boards[i];
		GString *label = g_string_new (b->name->str);
		g_string_append_printf (label, " (%s)",
			contract_string (b->level, b->trumps, b->declarer, b->doubled));
		GtkWidget *menuitem = gtk_radio_menu_item_new_with_label (group, label->str);
		g_string_free (label, TRUE);

		group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menuitem));
		if (i == win->cur)
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (menuitem), TRUE);
		win->boards[i]->n = i;
		g_signal_connect (G_OBJECT (menuitem), "activate",
			G_CALLBACK (board_menu_select), &(win->boards[i]->n));

		gtk_menu_shell_append (GTK_MENU_SHELL (submenu), menuitem);
		gtk_widget_show (menuitem);
	}
}

void show_board (board *b, redraw_t redraw)
{
	GtkWidget *w;
	GString *str = g_string_new(NULL);
	if (!b)
		return;

	if (redraw & REDRAW_BOARD_LIST)
		board_window_rebuild_board_menu (win);

	if (redraw & REDRAW_TITLE) {
		g_string_printf(str, "Tenace - %s (%s)", b->name->str,
			contract_string(b->level, b->trumps, b->declarer, b->doubled));
		if (win->title) {
			g_string_append_printf (str, " - %s", win->title->str);
		} else if (win->filename) {
			char *fname = win->filename->str;
			if (strrchr(fname, '/'))
				fname = strrchr(fname, '/') + 1;
			g_string_append_printf (str, " - %s", fname);
		}
		gtk_window_set_title(GTK_WINDOW(win->window), str->str);
	}

	if (redraw & REDRAW_CONTRACT) {
		w = lookup_widget(win->window, "label_board");
		g_string_printf(str, "%s\n%s", b->name->str,
			contract_string(b->level, b->trumps, b->declarer, b->doubled));
		gtk_label_set_text((GtkLabel*) w, str->str);

		char *dealermenu[] = { 0, "dealer_west1", "dealer_north1",
			"dealer_east1", "dealer_south1"};
		w = lookup_widget(win->window, dealermenu[b->dealer]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *declarermenu[] = { 0, "declarer_west1", "declarer_north1",
			"declarer_east1", "declarer_south1"};
		w = lookup_widget(win->window, declarermenu[b->declarer]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *levelmenu[] = { 0, "level1", "level2", "level3",
			"level4", "level5", "level6", "level7" };
		w = lookup_widget(win->window, levelmenu[b->level]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *suitmenu[] = { "contract_clubs1", "contract_diamonds1",
			"contract_hearts1", "contract_spades1", "contract_no_trump1" };
		w = lookup_widget(win->window, suitmenu[b->trumps]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *doublemenu[] = { "level_doubled0", "level_doubled1", "level_redoubled1" };
		w = lookup_widget(win->window, doublemenu[b->doubled]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *vulnmenu[] = { "vuln_none", "vuln_ns", "vuln_ew", "vuln_all" };
		w = lookup_widget(win->window, vulnmenu[2 * b->vuln[1] + b->vuln[0]]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);
	}

	if (redraw & REDRAW_NAMES) {
		w = lookup_widget(win->window, "label_west");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[1] ? "red" : "green",
			b->current_turn == west ? " weight=\"bold\"" : "",
			b->hand_name[0]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);

		w = lookup_widget(win->window, "label_north");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[0] ? "red" : "green",
			b->current_turn == north ? " weight=\"bold\"" : "",
			b->hand_name[1]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);

		w = lookup_widget(win->window, "label_east");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[1] ? "red" : "green",
			b->current_turn == east ? " weight=\"bold\"" : "",
			b->hand_name[2]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);

		w = lookup_widget(win->window, "label_south");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[0] ? "red" : "green",
			b->current_turn == south ? " weight=\"bold\"" : "",
			b->hand_name[3]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);
	}

	if (redraw & REDRAW_TRICKS) {
		w = lookup_widget(win->window, "label_tricks");
		g_string_printf(str, "NS: %d\nEW: %d", b->tricks[0], b->tricks[1]);
		gtk_label_set_markup((GtkLabel*) w, str->str);
	}

	if (redraw & REDRAW_DD) {
		if (b->par_score == -1) {
			w = lookup_widget(win->window, "par_label");
			gtk_label_set_text(GTK_LABEL(w), "");
		}

		if (run_dd)
			hilight_dd_scores(b);
	}

	if (redraw & REDRAW_HANDS) {
		int i, c;
		for (i = west; i <= south; i++) {
			for (c = 51; c >= 0; c--) {
				int has = i == b->cards[c];
				int had = i == b->dealt_cards[c];
				int color = has ? HAND_DISPLAY_CARD :
					(had && win->show_played_cards ?
					 HAND_DISPLAY_OLD_CARD : HAND_DISPLAY_NO_CARD);
				hand_display_set_card (win->handdisp[i - 1], c, color);
				if (has && b->card_score[c] >= 0)
					hand_display_set_card_score (win->handdisp[i - 1], c,
						card_overtricks(b, c));
			}
			hand_display_set_card_score_neg (win->handdisp[i - 1],
					i % 2 != b->declarer % 2);
			hand_display_draw(GTK_WIDGET (win->handdisp[i - 1]));
		}

		hand_display_table_reset_cards (win->table);
		if (b->n_played_cards) {
			int trick_start = b->n_played_cards - seat_mod(b->n_played_cards);
			for (i = trick_start; i < b->n_played_cards; i++) {
				card c = b->played_cards[i];
				seat s = b->dealt_cards[c];
				hand_display_table_set_card (win->table, i - trick_start, s, c);
			}
		}
		hand_display_draw(GTK_WIDGET (win->table));

		line_entry_set_from_board(b);
	}

	g_string_free(str, TRUE);

	if (redraw & REDRAW_PLAY)
		window_play_update(b);

	if (redraw & REDRAW_BIDDING) {
		int i;
		for (i = 0; i < b->n_bids; i++)
			printf ("%s\n", bid_string(b->bidding[i])->str);
	}
}

static void card_clicked (HandDisplay *handdisp, int *cp, int *seatp)
{
	board *b = CUR_BOARD;
	printf("Clicked: %s for %c.\n", card_string(*cp)->str, "WNES"[*seatp - 1]);
	if (play_card(b, b->cards[*cp], *cp))
		show_board(b, REDRAW_HANDS | REDRAW_TRICKS);
}

static void card_enter (HandDisplay *handdisp, int *cp, int *seatp)
{
	char buf[100];

	board_statusbar(NULL);

	board *b = CUR_BOARD;
	if (b->card_score[*cp] < 0)
		return;

	snprintf(buf, 99, "%s: %s",
		card_string(*cp)->str,
		score_string(b->level, b->trumps, b->declarer, b->doubled, b->vuln[b->declarer % 2],
			b->card_score[*cp], b->current_turn));
	board_statusbar(buf);
}

static void card_leave (HandDisplay *handdisp, int *cp, int *seatp)
{
	board_statusbar(NULL);
}

static void create_hand_widgets (window_board_t *win)
{
	static const char *alignment_a[] = {"alignment_w", "alignment_n", "alignment_e", "alignment_s"};
	static int dir[] = { 1, 2, 3, 4 };
	int h;
	GtkSizeGroup *sizegroup = gtk_size_group_new (GTK_SIZE_GROUP_BOTH);

	for (h = 0; h < 4; h++) {
		GtkWidget *alignment = lookup_widget(win->window, alignment_a[h]);
		GtkWidget *hand = hand_display_new(HAND_DISPLAY_MODE_HAND);
		gtk_container_add(GTK_CONTAINER(alignment), hand);
		gtk_widget_show(hand);
		g_signal_connect (hand, "card-clicked", G_CALLBACK (card_clicked), dir + h);
		g_signal_connect (hand, "card-enter", G_CALLBACK (card_enter), dir + h);
		g_signal_connect (hand, "card-leave", G_CALLBACK (card_leave), dir + h);
		win->handdisp[h] = HAND_DISPLAY(hand);
		gtk_size_group_add_widget (sizegroup, hand);
	}
	g_object_unref (sizegroup);

	GtkWidget *grid = lookup_widget (win->window, "table1");
	GtkWidget *table = hand_display_new (HAND_DISPLAY_MODE_TABLE);
	gtk_table_attach_defaults (GTK_TABLE (grid), table, 4, 5, 1, 2);
	gtk_widget_show (table);
	win->table = table;
}

void board_window_set_style (window_board_t *win, int style)
{
	int h;
	for (h = 0; h < 4; h++) {
		hand_display_set_style(win->handdisp[h], style, NULL);
	}
	hand_display_set_style(win->table, style, NULL);
	show_board(CUR_BOARD, REDRAW_HANDS);
}

int
board_window_append_board (window_board_t *win, board *b)
{
	if (!b) {
		b = board_new ();
		g_string_printf (b->name, "Board %d", win->n_boards + 1);
	}

	if (win->n_boards >= win->n_boards_alloc) {
		win->n_boards_alloc <<= 2;
		win->boards = realloc(win->boards, win->n_boards_alloc * sizeof (board*));
		assert(win->boards);
	}
	win->boards[win->n_boards] = b;
	return win->n_boards++;
}

void
board_window_init (window_board_t *win)
{
	win->window = create_window_hand ();
	win->statusbar = GTK_STATUSBAR (lookup_widget(win->window, "statusbar1"));
	win->board_menu = lookup_widget(win->window, "board_menu1");
	create_hand_widgets(win);

	win->show_played_cards = 0;
	win->card_width = 80;

	win->filename = NULL;
	win->title = NULL;
	win->boards = calloc(4, sizeof(board*));
	assert (win->boards);
	win->n_boards_alloc = 4;
	win->n_boards = 0;

	win->cur = 0;

	GdkColor bg = { 0, HAND_DISPLAY_TABLE_GDK_BG };
	gdk_colormap_alloc_color (gdk_colormap_get_system (), &bg, FALSE, TRUE);
	gtk_widget_modify_bg (win->window, GTK_STATE_NORMAL, &bg);

	gtk_widget_show (win->window);
}

void board_statusbar (char *text)
{
	static guint id = 0;
	if (!id)
		id = gtk_statusbar_get_context_id(win->statusbar, "window_board_c");

	gtk_statusbar_pop(win->statusbar, id);
	if (text)
		gtk_statusbar_push(win->statusbar, id, text);
}

void
board_set_declarer (seat declarer)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	board_rewind(b);
	b->declarer = declarer;
	b->current_turn = seat_mod(declarer + 1);
	show_board(b, REDRAW_CONTRACT | REDRAW_TRICKS | REDRAW_HANDS | REDRAW_NAMES);
	// FIXME: redraw less?
	PROTECT_END;
}

void
board_set_dealer (seat dealer)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->dealer = dealer;
	// FIXME: redraw something?
	PROTECT_END;
}

void
board_set_trumps (suit trumps)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->trumps = trumps;
	show_board(b, REDRAW_CONTRACT);
	PROTECT_END;
}

void
board_set_level (int level)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->level = level;
	calculate_target(b);
	show_board(b, REDRAW_CONTRACT);
	PROTECT_END;
}

void
board_set_vuln (int ns, int ew)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->vuln[0] = ns;
	b->vuln[1] = ew;
	b->par_score = -1;
	show_board(b, REDRAW_CONTRACT | REDRAW_NAMES);
	PROTECT_END;
}

void
board_set_doubled (int doubled)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->doubled = doubled;
	show_board(b, REDRAW_CONTRACT);
	PROTECT_END;
}

int
read_config (window_board_t *win)
{
	if (!rcfile) {
		char *home = getenv ("HOME");
		if (!home) {
			printf ("HOME unset, cannot read config");
			return 0;
		}
		rcfile = g_string_new (NULL);
		g_string_printf (rcfile, "%s/.tenacerc", home);
	}
	FILE *f;
	if (!(f = fopen(rcfile->str, "r"))) {
		return 0;
	}
	int style;
	if (fscanf (f, "style %d", &style) == 1) {
		char *checkitem_name[] = { "style_text", "style_cards" };
		GtkWidget *checkitem = lookup_widget (win->window, checkitem_name[style > 0]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (checkitem), TRUE);
	}
	fclose (f);
	return 1;
}

int
write_config (window_board_t *win)
{
	if (!rcfile) {
		return 0;
	}
	FILE *f;
	if (!(f = fopen(rcfile->str, "w"))) {
		perror (rcfile->str);
		return 0;
	}
	GtkWidget *checkitem = lookup_widget (win->window, "style_cards");
	fprintf (f, "style %d\n",
		gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (checkitem)));
	fclose (f);
	return 1;
}
