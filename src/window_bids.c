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

#include "window_board.h"

static GtkWidget *window_bids = 0;
static GtkTable *bids_table;
//static GtkButton *bids_button[52];

void
window_bids_init ()
{
	if (window_bids)
		return;

	window_bids = create_window_bids ();
	gtk_widget_show (window_bids);
	bids_table = GTK_TABLE(lookup_widget(window_bids, "bids_table"));
	assert (bids_table);
	GtkWidget *lab;

	lab = gtk_button_new_with_label ("PASS");
	gtk_table_attach (bids_table, lab, 0, 3, 0, 1, GTK_FILL, 0, 0, 0);
	lab = gtk_button_new_with_label ("X");
	gtk_table_attach (bids_table, lab, 3, 4, 0, 1, GTK_FILL, 0, 0, 0);
	lab = gtk_button_new_with_label ("XX");
	gtk_table_attach (bids_table, lab, 4, 5, 0, 1, GTK_FILL, 0, 0, 0);

	int d, l;
	for (l = 1; l <= 7; l++) {
		for (d = 0; d <= 4; d++) {
			GString *b = bid_string (5 * l + d);
			lab = gtk_button_new_with_label (b->str);
			gtk_table_attach(bids_table, lab, d, d+1, l, l+1, GTK_FILL, 0, 0, 0);
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
