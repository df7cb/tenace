/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2009 Christoph Berg <cb@df7cb.de>
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <assert.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "functions.h" /* _() */
#include "solve.h" /* run_dd */
#include "window_board.h"

/* menu */

G_MODULE_EXPORT void
on_neu1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	int i;
	for (i = 0; i < win->n_boards; i++) {
		board_free (win->boards[i]);
	}
	win->n_boards = 0;
	win->cur = board_window_append_board (win, NULL);
	card_window_update(win->boards[0]->dealt_cards);
	show_board(win->boards[0], REDRAW_FULL);
}


G_MODULE_EXPORT void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_load_dialog (win, 0);
}

G_MODULE_EXPORT void
on_append1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_load_dialog (win, 1);
}


G_MODULE_EXPORT void
on_speichern1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_save_dialog (win, 0);
}


G_MODULE_EXPORT void
on_speichern_unter1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_save_dialog (win, 1);
}


G_MODULE_EXPORT void
on_beenden1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	gtk_main_quit ();
}



G_MODULE_EXPORT void
on_button_hand_open_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board_load_dialog (win, 0);
}


G_MODULE_EXPORT void
on_button_hand_save_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board_save_dialog(win, 0);
}


G_MODULE_EXPORT void
on_declarer_west1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_declarer (west);
}


G_MODULE_EXPORT void
on_declarer_north1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_declarer (north);
}


G_MODULE_EXPORT void
on_declarer_east1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_declarer (east);
}


G_MODULE_EXPORT void
on_declarer_south1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_declarer (south);
}


G_MODULE_EXPORT void
on_contract_no_trump1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_trumps(NT);
}


G_MODULE_EXPORT void
on_contract_spades1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_trumps(spade);
}


G_MODULE_EXPORT void
on_contract_hearts1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_trumps(heart);
}


G_MODULE_EXPORT void
on_contract_diamonds1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_trumps(diamond);
}


G_MODULE_EXPORT void
on_contract_clubs1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_trumps(club);
}


G_MODULE_EXPORT void
on_level1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (1);
}


G_MODULE_EXPORT void
on_level2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (2);
}


G_MODULE_EXPORT void
on_level3_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (3);
}


G_MODULE_EXPORT void
on_level4_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (4);
}


G_MODULE_EXPORT void
on_level5_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (5);
}


G_MODULE_EXPORT void
on_level6_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (6);
}


G_MODULE_EXPORT void
on_level7_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_level (7);
}

G_MODULE_EXPORT void
on_level_doubled0_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_doubled (0);
}

G_MODULE_EXPORT void
on_level_doubled1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_doubled (1);
}


G_MODULE_EXPORT void
on_level_redoubled1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_doubled (2);
}


G_MODULE_EXPORT void
on_double_dummy1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	compute_dd_scores (b, 1);
	show_board(b, REDRAW_HANDS);
}


G_MODULE_EXPORT void
on_dd_always1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	run_dd = gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (menuitem));
	compute_dd_scores (b, run_dd);
	show_board (b, REDRAW_HANDS);
}


G_MODULE_EXPORT void
on_parscore1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	parscore(b);
	show_board (b, REDRAW_PAR);
}


G_MODULE_EXPORT void
on_handbutton_par_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	parscore(b);
	show_board (b, REDRAW_PAR);
}


G_MODULE_EXPORT void
on_vuln_none_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_vuln (0, 0);
}


G_MODULE_EXPORT void
on_vuln_ns_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_vuln (1, 0);
}


G_MODULE_EXPORT void
on_vuln_ew_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_vuln (0, 1);
}


G_MODULE_EXPORT void
on_vuln_all_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_vuln (1, 1);
}


G_MODULE_EXPORT void
on_set_par1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	parscore (b);
	if (b->trumps != b->par_suit || b->declarer != b->par_dec) {
		board_rewind (b); /* incompatible change */
		b->trumps = b->par_suit;
		b->declarer = b->par_dec;
		b->current_turn = seat_mod (b->declarer + 1);
	}
	b->level = b->par_level;
	b->doubled = b->par_tricks < b->par_level + 6;
	show_board (b, REDRAW_FULL);
}


G_MODULE_EXPORT void
on_rewind_play1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (!rewind_card (b)) /* extra call to show warning if there's nothing to do */
		return;
	board_rewind (b);
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
}


G_MODULE_EXPORT void
on_complete_play1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (!next_card (b)) /* extra call to show warning if there's nothing to do */
		return;
	board_fast_forward (b);
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
}


G_MODULE_EXPORT void
on_deal_clear_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	board_clear(b);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PLAY | REDRAW_PAR);
}


G_MODULE_EXPORT void
on_deal_random_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	deal_random(b);
	card_window_update(b->dealt_cards);
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}




G_MODULE_EXPORT void
on_dealer_west1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_dealer (west);
}


G_MODULE_EXPORT void
on_dealer_north1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_dealer (north);
}


G_MODULE_EXPORT void
on_dealer_east1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_dealer (east);
}


G_MODULE_EXPORT void
on_dealer_south1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_set_dealer (south);
}


G_MODULE_EXPORT void
on_played_cards1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	win->show_played_cards = gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (menuitem));
	show_board (win->boards[win->cur], REDRAW_HANDS);
}


G_MODULE_EXPORT void
on_deal_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	win->cur = board_window_append_board (win, NULL);
	card_window_update(win->boards[win->cur]->dealt_cards);
	show_board(win->boards[win->cur], REDRAW_FULL);
}


G_MODULE_EXPORT void
on_deal_cut_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (! win->n_boards) {
		board_statusbar (_("No board"));
		return;
	}
	if (win->cutbuf)
		board_free (win->cutbuf);
	board *b = CUR_BOARD;
	win->cutbuf = b;
	int i;
	for (i = win->cur; i < win->n_boards - 1; i++)
		win->boards[i] = win->boards[i + 1];
	win->n_boards--;
	win->boards[win->n_boards] = NULL;
	if (! win->n_boards) { /* last board was cut */
		board_window_append_board (win, NULL);
	}
	if (win->cur == win->n_boards)
		win->cur--;
	show_board (win->boards[win->cur], REDRAW_FULL);
}


G_MODULE_EXPORT void
on_deal_copy_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (! win->n_boards) {
		board_statusbar (_("No board"));
		return;
	}
	if (win->cutbuf)
		board_free (win->cutbuf);
	board *b = CUR_BOARD;
	win->cutbuf = board_dup (b);
	assert (b);
}


G_MODULE_EXPORT void
on_deal_paste_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (! win->cutbuf) {
		board_statusbar (_("Cut buffer is empty"));
		return;
	}

	board *b = board_dup (win->cutbuf);
	win->cur = board_window_append_board (win, b);
	g_string_printf (b->name, _("Board %d"), win->cur + 1);
	show_board (win->boards[win->cur], REDRAW_FULL);
}


G_MODULE_EXPORT void
on_ew1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	flip_hands (b, east, west);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR); /* flip invalidates parscore */
}


G_MODULE_EXPORT void
on_ns1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	flip_hands (b, north, south);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}


G_MODULE_EXPORT void
on_nw1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	flip_hands (b, north, west);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}


G_MODULE_EXPORT void
on_ne1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	flip_hands (b, north, east);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}


G_MODULE_EXPORT void
on_sw1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	flip_hands (b, south, west);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}


G_MODULE_EXPORT void
on_se1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	flip_hands (b, south, east);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}



/* toolbar */

G_MODULE_EXPORT void
on_rewind_button_clicked               (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (!rewind_card(b)) /* extra call to show warning if there's nothing to do */
		return;
	while (b->n_played_cards % 4 != 0)
		rewind_card (b);
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
}


G_MODULE_EXPORT void
on_button_back_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (!rewind_card(b))
		return;
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
}


G_MODULE_EXPORT void
on_button_next_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (!next_card(b))
		return;
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
}


G_MODULE_EXPORT void
on_button_fast_forward_clicked         (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (!next_card (b)) /* extra call to show warning if there's nothing to do */
		return;
	while (b->n_played_cards % 4 != 0) {
		if (! next_card (b))
			break;
	}
	compute_dd_scores (b, run_dd);
	show_board(b, REDRAW_HANDS | REDRAW_NAMES | REDRAW_TRICKS | REDRAW_PLAY);
}


G_MODULE_EXPORT void
on_button_dd_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	compute_dd_scores (b, 1);
	show_board(b, REDRAW_HANDS);
}


G_MODULE_EXPORT void
on_button_prev_board_clicked           (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	if (win->cur == 0) {
		board_statusbar (_("No previous board"));
		return;
	}
	PROTECT_BEGIN;
	board_statusbar (NULL);
	win->cur--;
	show_board (win->boards[win->cur], REDRAW_FULL);
	PROTECT_END;
}


G_MODULE_EXPORT void
on_button_next_board_clicked           (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	if (win->cur == win->n_boards - 1) {
		board_statusbar (_("No next board"));
		return;
	}
	PROTECT_BEGIN;
	board_statusbar (NULL);
	win->cur++;
	assert (0 <= win->cur && win->cur < win->n_boards);
	show_board (win->boards[win->cur], REDRAW_FULL);
	PROTECT_END;
}

