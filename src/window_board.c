/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2008 Christoph Berg <cb@df7cb.de>
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
#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../handdisplay/hand_display.h"

#include "window_board.h"

#include "file.h"
#include "functions.h"
#include "options.h"
#include "solve.h"
#include "window_card.h"
#include "window_line_entry.h"
#include "window_play.h"

window_board_t *win; // FIXME static?
int protect = 0;

static GdkColor bidding_non_vuln = { 0, 0.8*65535, 0.8*65535, 0.8*65535 };
static GdkColor bidding_vuln = { 0, 0.8*65535, 0, 0 };

static void
board_menu_select (GtkWidget *menuitem, int *n)
{
	PROTECT_BEGIN;
	if (!gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (menuitem)))
		PROTECT_RETURN;
	if (*n == win->cur)
		PROTECT_RETURN;
	win->cur = *n;
	//printf ("jump to %d\n", *n);
	assert (0 <= *n && *n < win->n_boards);
	show_board (win->boards[*n], REDRAW_BOARD);
	PROTECT_END;
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

static void
board_set_player_name (GtkWidget *w, seat s, int dealer, int vuln,
		int declarer, int current, char *name)
{
	GString *str = g_string_new (NULL);
	g_string_printf(str, "<span background=\"%s\"%s> %s </span><span%s%s> %s </span>",
		vuln ? "#cc0000" : "#00cc00",
		dealer ? " weight=\"bold\"" : "",
		_(seat_str[s]),
		declarer ? " weight=\"bold\"" : "",
		current ? " background=\"#ffa000\"" : "",
		name);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	g_string_free (str, TRUE);
}

static void
bidding_update (window_board_t *win, board *b, int scroll)
{
	gtk_list_store_clear (win->bidding_store);

	int i;
	for (i = 0; i < 2; i++) {
		GdkColor *color = b->vuln[!i] ? &bidding_vuln : &bidding_non_vuln;
		gtk_widget_modify_bg (win->bidding_header[i], GTK_STATE_NORMAL, color);
		gtk_widget_modify_bg (win->bidding_header[i+2], GTK_STATE_NORMAL, color);
	}
	/*
	for (i = 0; i < 4; i++) {
		if (b->hand_name[i])
			gtk_label_set_text (GTK_LABEL (win->bidding_label[i]), b->hand_name[i]->str);
	}
	*/

	int col = b->dealer - 1;
	GtkTreeIter iter;

	if (!b->n_bids) {
		gtk_list_store_append (win->bidding_store, &iter);
		gtk_list_store_set (win->bidding_store, &iter,
				2 * col, "•",
				2 * col + 1, _("Dealer"),
				-1);
		return;
	}

	int last_col = 5;
	for (i = 0; i < b->n_bids; i++) {
		char buf[100];
		if (last_col > col)
			gtk_list_store_append (win->bidding_store, &iter);
		snprintf (buf, sizeof (buf), "%s%s", bid_string(b->bidding[i], b->alerts[i] != NULL)->str,
				b->alerts[i] ? (*b->alerts[i] ? _("*") : _("!")) : "");
		gtk_list_store_set (win->bidding_store, &iter,
				2 * col, buf,
				2 * col + 1, b->alerts[i],
				-1);
		last_col = col;
		col = (col + 1) % 4;
	}

	if (scroll) {
		GtkTreePath *path = gtk_tree_model_get_path
			(GTK_TREE_MODEL (win->bidding_store), &iter);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (win->bidding), path,
			NULL, FALSE, 0.0, 0.0);
		gtk_tree_path_free (path);
	}
}

void show_board (board *b, redraw_t redraw)
{
	GtkWidget *w;
	GString *str = g_string_new(NULL);
	assert (b);

	if (redraw & REDRAW_BOARD_LIST) {
		board_window_rebuild_board_menu (win);
		window_options_board_list_populate ();
	}

	if (redraw & (REDRAW_TITLE | REDRAW_CONTRACT)) {
		g_string_printf(str, "Tenace - %s (%s)", b->name->str,
			contract_string_asc (b->level, b->trumps, b->declarer, b->doubled));
		if (win->title) {
			g_string_append_printf (str, " - %s", win->title);
			if (win->subtitle)
				g_string_append_printf (str, " - %s", win->subtitle);
			if (win->team1 && win->team2)
				g_string_append_printf (str, _(" - %s vs. %s"),
						win->team1, win->team2);
		} else if (win->filename) {
			char *fname = win->filename;
			if (strrchr(fname, '/'))
				fname = strrchr(fname, '/') + 1;
			g_string_append_printf (str, " - %s", fname);
		}
		gtk_window_set_title(GTK_WINDOW(win->window), str->str);
	}

	if (redraw & REDRAW_CONTRACT) {
		w = glade_xml_get_widget (win->xml, "label_board");
		g_string_printf(str, "%s\n%s", b->name->str,
			contract_string(b->level, b->trumps, b->declarer, b->doubled));
		gtk_label_set_text((GtkLabel*) w, str->str);

		char *dealermenu[] = { 0, "dealer_west1", "dealer_north1",
			"dealer_east1", "dealer_south1"};
		w = glade_xml_get_widget (win->xml, dealermenu[b->dealer]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *declarermenu[] = { 0, "declarer_west1", "declarer_north1",
			"declarer_east1", "declarer_south1"};
		w = glade_xml_get_widget (win->xml, declarermenu[b->declarer]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *levelmenu[] = { "level1" /* PASS */, "level1", "level2", "level3",
			"level4", "level5", "level6", "level7" };
		w = glade_xml_get_widget (win->xml, levelmenu[b->level]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *suitmenu[] = { "contract_clubs1", "contract_diamonds1",
			"contract_hearts1", "contract_spades1", "contract_no_trump1" };
		w = glade_xml_get_widget (win->xml, suitmenu[b->trumps]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *doublemenu[] = { "level_doubled0", "level_doubled1", "level_redoubled1" };
		w = glade_xml_get_widget (win->xml, doublemenu[b->doubled]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		char *vulnmenu[] = { "vuln_none", "vuln_ns", "vuln_ew", "vuln_all" };
		w = glade_xml_get_widget (win->xml, vulnmenu[2 * b->vuln[1] + b->vuln[0]]);
		gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM (w), TRUE);

		window_options_board_populate ();
	}

	if (redraw & REDRAW_NAMES) {
		w = glade_xml_get_widget (win->xml, "label_west");
		board_set_player_name (w, west, b->dealer == west, b->vuln[1],
				b->declarer == west,
				b->current_turn == west, b->hand_name[0]->str);
		w = glade_xml_get_widget (win->xml, "label_north");
		board_set_player_name (w, north, b->dealer == north, b->vuln[0],
				b->declarer == north,
				b->current_turn == north, b->hand_name[1]->str);
		w = glade_xml_get_widget (win->xml, "label_east");
		board_set_player_name (w, east, b->dealer == east, b->vuln[1],
				b->declarer == east,
				b->current_turn == east, b->hand_name[2]->str);
		w = glade_xml_get_widget (win->xml, "label_south");
		board_set_player_name (w, south, b->dealer == south, b->vuln[0],
				b->declarer == south,
				b->current_turn == south, b->hand_name[3]->str);

		window_options_board_populate ();
	}

	if (redraw & REDRAW_TRICKS) {
		w = glade_xml_get_widget (win->xml, "label_tricks");
		g_string_printf(str, _("NS: %d\nEW: %d"), b->tricks[0], b->tricks[1]);
		gtk_label_set_markup((GtkLabel*) w, str->str);
	}

	if (redraw & REDRAW_PAR) {
		w = glade_xml_get_widget (win->xml, "par_label");
		if (b->par_score == -1) {
			gtk_label_set_text (GTK_LABEL (w), "");
		} else {
			char *par = par_label (b);
			gtk_label_set_markup(GTK_LABEL (w), par);
			free (par);
		}
	}

	if (redraw & REDRAW_HANDS) {
		int i, c;
		/* hands */
		int next_card = b->played_cards[b->n_played_cards];
		for (i = west; i <= south; i++) {
			for (c = 51; c >= 0; c--) {
				int has = i == b->cards[c];
				int had = i == b->dealt_cards[c];
				int color;
				if (seat_mask (i, win->show_hands))
					color = has ?
					(c == next_card ?
					 HAND_DISPLAY_HILIGHT_CARD : HAND_DISPLAY_CARD) :
					(had && win->show_played_cards ?
					 HAND_DISPLAY_OLD_CARD : HAND_DISPLAY_NO_CARD);
				else
					color = HAND_DISPLAY_NO_CARD;
				hand_display_set_card (win->handdisp[i - 1], c, color);
				if (has && b->current_dd && b->current_dd->card_score[c] >= 0 &&
						seat_mask (i, win->show_dd_scores))
					hand_display_set_card_score (win->handdisp[i - 1], c,
						card_overtricks(b, c));
				else
					hand_display_set_card_score (win->handdisp[i - 1], c,
						HAND_DISPLAY_NO_SCORE);
			}
			hand_display_set_card_score_neg (win->handdisp[i - 1],
					i % 2 != b->declarer % 2);
			hand_display_draw(GTK_WIDGET (win->handdisp[i - 1]));
		}

		/* table */
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
		bidding_update (win, b, redraw & REDRAW_BIDDING_SCROLL);
	}
}

void
recently_used_add (char *filename)
{
#if GTK_CHECK_VERSION (2,10,0)
	static GtkRecentManager *recent = NULL;
	if (! recent)
		recent = gtk_recent_manager_get_default ();

	char buf[1024];
	snprintf (buf, sizeof (buf), "file://%s", filename);
	gtk_recent_manager_add_item (recent, buf);
#endif
}

/* callbacks */

static void card_clicked (HandDisplay *handdisp, int card, int *seatp)
{
	if (card == -1)
		return;

	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	assert (card >= 0 && card < 52);
	//printf("Clicked: %s for %c.\n", card_string(card), "WNES"[*seatp - 1]);
	int redraw = 0;

	if (*seatp != b->current_turn && b->n_played_cards > 0 &&
		b->dealt_cards[b->played_cards[b->n_played_cards - 1]] == *seatp) { /* quick undo */
		rewind_card (b);
		redraw = 1;
	}

	if (play_card(b, *seatp, card)) {
		redraw = 1;
	}

	if (redraw) {
		compute_dd_scores (b, run_dd);
		show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
	}

	PROTECT_END;
}

static void card_enter (HandDisplay *handdisp, int card, int *seatp)
{
	PROTECT_BEGIN;
	char buf[100];

	board_statusbar(NULL);

	board *b = CUR_BOARD;
	if (!b->current_dd || b->current_dd->card_score[card] < 0 ||
			!seat_mask (*seatp, win->show_dd_scores))
		PROTECT_RETURN;

	snprintf(buf, 99, "%s: %s",
		card_string(card),
		score_string(b->level, b->trumps, b->declarer, b->doubled, b->vuln[b->declarer % 2],
			b->current_dd->card_score[card], b->current_turn));
	board_statusbar(buf);

	/* what-if */
	//hilight_next_dd_scores (b, card);
	//show_board (b, REDRAW_DD);
	PROTECT_END;
}

static void card_leave (HandDisplay *handdisp, int card, int *seatp)
{
	PROTECT_BEGIN;
	//board *b = CUR_BOARD;
	board_statusbar(NULL);
	//show_board (b, REDRAW_DD);
	PROTECT_END;
}

/* gets called for target widget */
static void
card_drag_drop (HandDisplay *handdisp, int card, int on_card, int *to_seat)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	printf("Dropped: %s on seat %c.\n", card_string(card), "WNES"[*to_seat - 1]);
	if (on_card >= 0)
		printf("Dropped on: %s.\n", card_string(on_card));

	if (b->dealt_cards[card] && b->dealt_cards[card] == *to_seat) /* card didn't move */
		PROTECT_RETURN;

	if (b->dealt_cards[card] && !b->cards[card]) {
		board_statusbar(_("Card is in play and cannot be moved"));
		PROTECT_RETURN;
	}

	seat from_seat = b->dealt_cards[card];
	if (on_card >= 0) { /* exchange 2 cards */
		if (b->dealt_cards[on_card] && !b->cards[on_card]) {
			board_statusbar(_("Card is in play and cannot be exchanged"));
			PROTECT_RETURN;
		}

		remove_card(b, *to_seat, on_card);
		if (from_seat) {
			remove_card(b, from_seat, card);
			add_card(b, from_seat, on_card);
		}
		add_card(b, *to_seat, card);
	} else { /* move single card */
		if (b->hand_cards[*to_seat-1] == 13) {
			board_statusbar(_("Hand has already 13 cards"));
			PROTECT_RETURN;
		}

		if (from_seat)
			remove_card(b, from_seat, card);
		add_card(b, *to_seat, card);
	}
	b->par_score = -1;
	invalidate_dd_scores (b);

	board_statusbar(NULL);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
	PROTECT_END;
}

/*
static void
bidding_clicked (GtkTreeViewColumn *column, void *data)
{
	printf ("clicked bidd\n");
}
*/

#if GTK_CHECK_VERSION (2,12,0)
static gboolean
bidding_query_tooltip (GtkWidget *widget, gint x, gint y, gboolean keyboard_mode,
		GtkTooltip *tooltip, window_board_t *bidding_store)
{
	if (keyboard_mode) {
		printf ("FIXME: keyboard usage\n");
		return FALSE;
	}
	GtkTreeIter iter;
	gboolean ret = gtk_tree_view_get_tooltip_context (GTK_TREE_VIEW (widget),
			&x, &y, keyboard_mode, NULL, NULL, &iter);
	if (!ret) {
		gtk_tooltip_set_text (tooltip, NULL);
		// TODO: don't show anything at all, or something interesting
		// y<0 -> headings
		return TRUE;
	}

	int i;
	int width = 0;
	for (i = 0; i < 4; i++) {
		width += gtk_tree_view_column_get_width (win->bidding_column[i]);
		// TODO: cache this, or use gtk_tree_view_get_path_at_pos
		if (x < width)
			break;
	}
	assert (i < 4);

	gchar *alert;
	gtk_tree_model_get (GTK_TREE_MODEL (win->bidding_store), &iter, 2 * i + 1, &alert, -1);
	gtk_tooltip_set_markup (tooltip, !alert || *alert ? alert : _("(no explanation)"));
	return TRUE;
}
#endif

#if GTK_CHECK_VERSION (2,10,0)
static void
jump_menu_select (GtkWidget *recentchooser, char *unused)
{
	char *filename = gtk_recent_chooser_get_current_uri (GTK_RECENT_CHOOSER (recentchooser));
	if (strncmp (filename, "file://", sizeof ("file://") - 1))
		return;
	board_load_popup (win, 0, filename + sizeof ("file://") - 1);
}
#endif

/* infrastructure */

static void create_hand_widgets (window_board_t *win)
{
	static const char *alignment_a[] = {"alignment_w", "alignment_n", "alignment_e", "alignment_s"};
	static int dir[] = { 1, 2, 3, 4 };
	int h;
	GtkSizeGroup *sizegroup = gtk_size_group_new (GTK_SIZE_GROUP_BOTH);

	for (h = 0; h < 4; h++) {
		GtkWidget *alignment = glade_xml_get_widget (win->xml, alignment_a[h]);
		GtkWidget *hand = hand_display_new(HAND_DISPLAY_MODE_HAND);
		gtk_container_add(GTK_CONTAINER(alignment), hand);
		gtk_widget_show(hand);
		g_signal_connect (hand, "card-clicked", G_CALLBACK (card_clicked), dir + h);
		g_signal_connect (hand, "card-enter", G_CALLBACK (card_enter), dir + h);
		g_signal_connect (hand, "card-leave", G_CALLBACK (card_leave), dir + h);
		g_signal_connect (hand, "card-drag-drop", G_CALLBACK (card_drag_drop), dir + h);
		win->handdisp[h] = HAND_DISPLAY(hand);
		hand_display_set_drag (HAND_DISPLAY (hand), 1);
		gtk_size_group_add_widget (sizegroup, hand);
	}
	g_object_unref (sizegroup);

	/* trick display */
	GtkWidget *grid = glade_xml_get_widget (win->xml, "table1");
	GtkWidget *table = hand_display_new (HAND_DISPLAY_MODE_TABLE);
	gtk_table_attach_defaults (GTK_TABLE (grid), table, 1, 2, 1, 2);
	gtk_widget_show (table);
	win->table = HAND_DISPLAY (table);
}

static void
create_bidding_widget_cb (GtkWidget *w, window_board_t *win)
{
	static int i = 0;
	win->bidding_header[i++] = w;
	assert (i <= 4);
}

static void
create_bidding_widget (window_board_t *win)
{
	/*
	GtkScrolledWindow *scroll = GTK_SCROLLED_WINDOW
		(glade_xml_get_widget (win->xml, "scrolledwindow2"));
	GdkColor bg = { 0, 0.8*65535, 0.0, 0.0 };
	gdk_colormap_alloc_color (gdk_colormap_get_system (), &bg, FALSE, TRUE);
	gtk_widget_modify_bg (GTK_WIDGET (scroll), GTK_STATE_NORMAL, &bg);
	*/

	win->bidding = GTK_TREE_VIEW (glade_xml_get_widget (win->xml, "treeview_bidding"));
	//gtk_widget_modify_bg (GTK_WIDGET (bidding), GTK_STATE_NORMAL, &bg);
	//gdk_window_set_background (gtk_tree_view_get_bin_window (bidding), &bidding_vuln);
	win->bidding_store = gtk_list_store_new (8,
		G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (win->bidding, GTK_TREE_MODEL (win->bidding_store));
#if GTK_CHECK_VERSION (2,12,0)
	g_signal_connect (G_OBJECT (win->bidding), "query-tooltip",
			G_CALLBACK (bidding_query_tooltip), win);
#endif

	GtkCellRenderer *renderer;
	renderer = gtk_cell_renderer_text_new ();
	g_object_set (renderer, "xalign", 0.5,
				"cell-background", "yellow",
				"mode", GTK_CELL_RENDERER_MODE_ACTIVATABLE,
				NULL);

	char *dir[] = {"W", "N", "E", "S"};
	int i;
	for (i = 0; i < 4; i++) {
		GtkTreeViewColumn *column;
		column = gtk_tree_view_column_new_with_attributes (_(dir[i]), renderer,
				"markup", 2 * i, NULL);
		gtk_tree_view_column_set_expand (column, TRUE);
		gtk_tree_view_column_set_min_width (column, 36);
		gtk_tree_view_column_set_alignment (column, 0.5);
		//g_signal_connect_swapped (column, "clicked", G_CALLBACK (bidding_clicked), 0);
		/*
		win->bidding_label[i] = gtk_label_new (dir[i]);
		//gtk_label_set_width_chars (win->bidding_label[i], 4);
		gtk_label_set_ellipsize (win->bidding_label[i], PANGO_ELLIPSIZE_END);
		gtk_tree_view_column_set_widget (column, win->bidding_label[i]);
		gtk_widget_show (win->bidding_label[i]);
		*/
		gtk_tree_view_append_column (win->bidding, column);
		win->bidding_column[i] = column;
	}

	gtk_container_forall (GTK_CONTAINER (win->bidding), (GtkCallback) create_bidding_widget_cb, win);

	GdkColormap *cmap = gdk_colormap_get_system ();
	gdk_colormap_alloc_color (cmap, &bidding_non_vuln, FALSE, TRUE);
	gdk_colormap_alloc_color (cmap, &bidding_vuln, FALSE, TRUE);
}

void board_window_set_style (window_board_t *win, int style)
{
	win->hand_display_style = style;
	int h;
	for (h = 0; h < 4; h++) {
		hand_display_set_style(win->handdisp[h], style);
	}
	hand_display_set_style(win->table, style);
	if (win->n_boards)
		show_board(CUR_BOARD, REDRAW_HANDS);
}

int
board_window_append_board (window_board_t *win, board *b)
{
	if (!b) {
		b = board_new ();
		g_string_printf (b->name, _("Board %d"), win->n_boards + 1);
	}

	if (! win->n_boards_alloc) {
		win->n_boards_alloc = 4;
		win->boards = malloc (win->n_boards_alloc * sizeof (board *));
		assert(win->boards);
	} else if (win->n_boards >= win->n_boards_alloc) {
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
	//win->window = create_window_hand ();
	win->window = glade_xml_get_widget (win->xml, "window_hand");
	win->statusbar = GTK_STATUSBAR (glade_xml_get_widget (win->xml, "statusbar1"));
	win->board_menu = glade_xml_get_widget (win->xml, "board_menu1");
	create_hand_widgets(win);
	create_bidding_widget (win);

	/* set up "recently used" menu */
	GtkWidget *jump_menu = glade_xml_get_widget (win->xml, "jump_to1");
#if GTK_CHECK_VERSION (2,10,0)
	GtkWidget *recentchooser = gtk_recent_chooser_menu_new ();
	gtk_recent_chooser_set_sort_type (GTK_RECENT_CHOOSER (recentchooser),
			GTK_RECENT_SORT_MRU);
	gtk_menu_item_set_submenu (GTK_MENU_ITEM (jump_menu), recentchooser);
	g_signal_connect (G_OBJECT (recentchooser), "item-activated",
			G_CALLBACK (jump_menu_select), NULL);
	GtkRecentFilter *filter = gtk_recent_filter_new ();
	gtk_recent_filter_add_pattern (filter, "*.lin");
	gtk_recent_filter_add_pattern (filter, "*.pbn");
	gtk_recent_chooser_add_filter (GTK_RECENT_CHOOSER (recentchooser), filter);
#else
	gtk_widget_set_sensitive (jump_menu, FALSE);
#endif

	win->keyfile = g_key_file_new ();
	win->show_played_cards = 0;
	win->show_hands = seat_all;
	win->show_dd_scores = seat_all;
	win->card_width = 70;

	win->filename = NULL;
	win->title = win->subtitle = win->team1 = win->team2 = NULL;
	win->boards = NULL;
	win->n_boards_alloc = 0;
	win->n_boards = 0;

	win->cutbuf = NULL;

	win->cur = 0;

	GdkColor bg = { 0, HAND_DISPLAY_TABLE_GDK_BG };
	gdk_colormap_alloc_color (gdk_colormap_get_system (), &bg, FALSE, TRUE);
	gtk_widget_modify_bg (win->window, GTK_STATE_NORMAL, &bg);

	gtk_widget_show (win->window);
}

G_MODULE_EXPORT gboolean
on_window_hand_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	gtk_main_quit ();
	return FALSE;
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
	board *b = CUR_BOARD;
	if (declarer == b->declarer)
		return;

	if (b->n_played_cards > 0) {
		board_statusbar (_("Cannot change declarer while cards are in play"));
		PROTECT_BEGIN;
		show_board(b, REDRAW_CONTRACT);
		PROTECT_END;
		return;
	}

	PROTECT_BEGIN;
	b->declarer = declarer;
	b->current_turn = seat_mod(declarer + 1);
	invalidate_dd_scores (b);
	show_board(b, REDRAW_CONTRACT | REDRAW_TRICKS | REDRAW_HANDS | REDRAW_NAMES | REDRAW_BOARD_LIST);
	PROTECT_END;
}

void
board_set_dealer (seat dealer)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->dealer = dealer;
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_BIDDING);
	PROTECT_END;
}

void
board_set_trumps (suit trumps)
{
	board *b = CUR_BOARD;
	if (trumps == b->trumps)
		return;

	if (b->n_played_cards > 0) {
		board_statusbar (_("Cannot change trumps while cards are in play"));
		PROTECT_BEGIN;
		show_board(b, REDRAW_CONTRACT);
		PROTECT_END;
		return;
	}

	PROTECT_BEGIN;
	board_rewind (b);
	b->trumps = trumps;
	invalidate_dd_scores (b);
	show_board(b, REDRAW_CONTRACT | REDRAW_BOARD_LIST);
	PROTECT_END;
}

void
board_set_level (int level)
{
	board *b = CUR_BOARD;
	if (level == b->level)
		return;

	PROTECT_BEGIN;
	b->level = level;
	invalidate_dd_scores (b);
	calculate_target(b);
	show_board(b, REDRAW_CONTRACT | REDRAW_BOARD_LIST);
	PROTECT_END;
}

void
board_set_vuln (int ns, int ew)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	if (ns == b->vuln[0] && ew == b->vuln[1])
		PROTECT_RETURN;
	b->vuln[0] = ns;
	b->vuln[1] = ew;
	b->par_score = -1;
	show_board(b, REDRAW_CONTRACT | REDRAW_NAMES | REDRAW_BIDDING | REDRAW_PAR);
	PROTECT_END;
}

void
board_set_doubled (int doubled)
{
	PROTECT_BEGIN;
	board *b = CUR_BOARD;
	b->doubled = doubled;
	show_board(b, REDRAW_CONTRACT | REDRAW_BOARD_LIST);
	PROTECT_END;
}

