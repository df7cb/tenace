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
		alert_entry = glade_xml_get_widget (win->xml, "alert_entry");

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

	window_bids = glade_xml_get_widget (win->xml, "window_bids");
	gtk_widget_show (window_bids);
	GtkTable *bids_table = GTK_TABLE(glade_xml_get_widget (win->xml, "bids_table"));
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
