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

#include "bridge.h"
#include "functions.h"
#include "interface.h"
#include "support.h"

static GtkWidget *window_bidding = 0;

static GtkWidget *window_bids = 0;
static GtkTable *bids_table;
static GtkButton *bids_button[52];

void
window_bidding_init ()
{
	if (window_bidding)
		return;

	window_bidding = create_window_bidding ();
	gtk_widget_show (window_bidding);
	/*
	bids_table = GTK_TABLE(lookup_widget(window_bids, "bids_table"));
	assert (bids_table);
	int cr, cc;
	for (cr = 0; cr <= 12; cr++) {
		char str[5];
		snprintf(str, 5, " %d ", cr+1);
		GtkWidget *lab = gtk_label_new(str);
		gtk_table_attach(bids_table, lab, 0, 1, cr, cr+1, 0, 0, 0, 0);
		for (cc = 0; cc <= 3; cc++) {
			GtkWidget *lab = gtk_label_new(NULL);
			gtk_table_attach(bids_table, lab, cc+1, cc+2, cr, cr+1, 0, 0, 0, 0);
			bids_label[4 * cr + cc] = GTK_LABEL(lab);
		}
	}
	*/
	gtk_widget_show_all (window_bidding);
}

void
window_bidding_delete (void)
{
	if (!window_bidding)
		return;

	gtk_widget_destroy (window_bidding);
	window_bidding = 0;
}


void
window_bids_init ()
{
	if (window_bids)
		return;

	window_bids = create_window_bids ();
	gtk_widget_show (window_bids);
	bids_table = GTK_TABLE(lookup_widget(window_bids, "bids_table"));
	assert (bids_table);
	int d, l;
	for (l = 1; l <= 7; l++) {
		for (d = 0; d <= 4; d++) {
			GString *b = bid_string (5 * l + d);
			GtkWidget *lab = gtk_button_new_with_label (b->str);
			gtk_table_attach(bids_table, lab, d, d+1, l-1, l, 0, 0, 0, 0);
			//bids_label[5 * l + d] = GTK_LABEL(lab);
		}
	}
	gtk_widget_show_all (window_bids);
}

void
window_bids_delete (void)
{
	if (!window_bids)
		return;

	gtk_widget_destroy (window_bids);
	window_bids = 0;
}
