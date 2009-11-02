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

#include <assert.h>
#include <string.h>

#include "bridge.h"
#include "functions.h"

#include "window_board.h"

static GtkWidget *window_bids = 0;

static void
bid_clicked (GtkWidget *lab, int *bid)
{
	static GtkWidget *alert_entry = NULL;
	if (! alert_entry)
		alert_entry = get_widget ("alert_entry");

	board *b = CUR_BOARD;
	int ret = board_append_bid (b, *bid, 1);

	if (!ret)
		return;

	PROTECT_BEGIN;
	const char *alert = gtk_entry_get_text (GTK_ENTRY (alert_entry));
	if (*alert) {
		board_set_alert (b, !strcmp (alert, "!") ? "" : alert);
		gtk_entry_set_text (GTK_ENTRY (alert_entry), "");
	}

	show_board (b, (ret == 2 ? REDRAW_FULL : REDRAW_BIDDING) | REDRAW_BIDDING_SCROLL);

	if (ret == 2) {
		char buf[50];
		snprintf (buf, sizeof (buf), _("Contract set to %s"),
			contract_string (b->level, b->trumps, b->declarer, b->doubled));
		board_statusbar (buf);
	}
	PROTECT_END;
}

void
window_bids_init ()
{
	if (window_bids)
		return;

	static int pass = bid_pass, x = bid_x, xx = bid_xx;
	static int bid[35];

	window_bids = get_widget ("window_bids");
	gtk_widget_show (window_bids);
	GtkTable *bids_table = GTK_TABLE(get_widget ("bids_table"));
	assert (bids_table);
	GtkWidget *lab;

	lab = gtk_button_new_with_label (_("PASS"));
	gtk_table_attach (bids_table, lab, 0, 3, 0, 1, GTK_FILL, 0, 0, 0);
	g_signal_connect (lab, "clicked", G_CALLBACK (bid_clicked), &pass);
	lab = gtk_button_new_with_label (_("X"));
	gtk_table_attach (bids_table, lab, 3, 4, 0, 1, GTK_FILL, 0, 0, 0);
	g_signal_connect (lab, "clicked", G_CALLBACK (bid_clicked), &x);
	lab = gtk_button_new_with_label (_("XX"));
	gtk_table_attach (bids_table, lab, 4, 5, 0, 1, GTK_FILL, 0, 0, 0);
	g_signal_connect (lab, "clicked", G_CALLBACK (bid_clicked), &xx);

	int d, l;
	for (l = 1; l <= 7; l++) {
		for (d = 0; d <= 4; d++) {
			GString *b = bid_string (5 * l + d, 0);
			lab = gtk_button_new_with_label (_(b->str));
			gtk_label_set_use_markup (GTK_LABEL (gtk_bin_get_child (GTK_BIN (lab))), TRUE);
			gtk_table_attach(bids_table, lab, d, d+1, l, l+1, GTK_FILL, 0, 0, 0);
			bid[5 * (l - 1) + d] = 5 * l + d;
			g_signal_connect (lab, "clicked",
					G_CALLBACK (bid_clicked), bid + (5 * (l - 1) + d));
		}
	}
	gtk_widget_show_all (window_bids);
}

void
window_bids_delete (void)
{
	if (!window_bids)
		return;

	gtk_widget_hide (window_bids);
	window_bids = 0;
}

G_MODULE_EXPORT void
on_bid_clear_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	board_clear_bidding (b);
	show_board(b, REDRAW_BIDDING);
}

G_MODULE_EXPORT void
on_bid_undo_clicked                    (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board *b = CUR_BOARD;
	if (b->n_bids) {
		board_remove_bid (b);
	}
	show_board(b, REDRAW_BIDDING | REDRAW_BIDDING_SCROLL);
}

#define TRY(x) { if (!(x)) goto end; }

G_MODULE_EXPORT void
on_bid_set_contract_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	int i;
	board *b = CUR_BOARD;
	seat cur_seat = seat_mod (b->dealer + b->n_bids);
	int passes = (b->declarer - cur_seat) % 4;

	for (i = (b->n_bids + passes - 2) % 4; i < b->n_bids; i += 4)
		if (b->bidding[i] >= 5 && DENOM (b->bidding[i]) == b->trumps) {
			board_statusbar (_("Suit was already bid from wrong side"));
			goto end;
		}

	for (i = 0; i < passes; i++)
		TRY (board_append_bid (b, bid_pass, 0));

	TRY (board_append_bid (b, 5 * b->level + b->trumps, 0));
	if (b->doubled)
		TRY (board_append_bid (b, bid_x, 0));
	if (b->doubled == bid_xx)
		TRY (board_append_bid (b, bid_xx, 0));
	TRY (board_append_bid (b, bid_pass, 0));
	TRY (board_append_bid (b, bid_pass, 0));
	TRY (board_append_bid (b, bid_pass, 0));

end:
	show_board(b, REDRAW_BIDDING | REDRAW_BIDDING_SCROLL);
}

G_MODULE_EXPORT gboolean
on_window_bids_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	PROTECT_BEGIN_BOOL;
	GtkCheckMenuItem *menuitem = GTK_CHECK_MENU_ITEM (get_widget ("bids1"));
	gtk_check_menu_item_set_active (menuitem, FALSE);
	window_bids_delete ();
	PROTECT_END;
	return FALSE;
}

G_MODULE_EXPORT void
on_bids1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (menuitem)))
		window_bids_init ();
	else
		window_bids_delete ();
}

