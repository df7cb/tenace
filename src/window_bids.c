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
static GtkTreeView *bidding = 0;
static GtkListStore *store = 0;

static GtkWidget *window_bids = 0;
static GtkTable *bids_table;
//static GtkButton *bids_button[52];

void
window_bidding_update (board *b)
{
	if (!window_bidding)
		return;

	printf ("updating bidding window...\n");

	gtk_list_store_clear (store);

	GtkTreeIter   iter;  /* Acquire an iterator */

	int i;
	int col = b->dealer - 1;
	int last_col = 5;

	for (i = 0; i < b->n_bids; i++) {
		if (last_col > col)
			gtk_list_store_append (store, &iter);
		gtk_list_store_set (store, &iter,
			col, bid_string(b->bidding[i])->str,
			-1);
		last_col = col;
		col = (col + 1) % 4;
	}
}

void
window_bidding_init (board *b)
{
	if (window_bidding)
		return;

	window_bidding = create_window_bidding ();
	gtk_widget_show (window_bidding);
	bidding = GTK_TREE_VIEW (lookup_widget(window_bidding, "treeview_bid"));
	assert (bidding);

	store = gtk_list_store_new (4, G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (bidding, GTK_TREE_MODEL (store));

	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new ();
	g_object_set (renderer, "xalign", 0.5, NULL);

	column = gtk_tree_view_column_new_with_attributes ("W", renderer, "text", 0, NULL);
	gtk_tree_view_column_set_min_width (column, 35);
	gtk_tree_view_column_set_alignment (column, 0.5);
	gtk_tree_view_append_column (bidding, column);

	column = gtk_tree_view_column_new_with_attributes ("N", renderer, "text", 1, NULL);
	gtk_tree_view_column_set_min_width (column, 35);
	gtk_tree_view_column_set_alignment (column, 0.5);
	gtk_tree_view_append_column (bidding, column);

	column = gtk_tree_view_column_new_with_attributes ("E", renderer, "text", 2, NULL);
	gtk_tree_view_column_set_min_width (column, 35);
	gtk_tree_view_column_set_alignment (column, 0.5);
	gtk_tree_view_append_column (bidding, column);

	column = gtk_tree_view_column_new_with_attributes ("S", renderer, "text", 3, NULL);
	gtk_tree_view_column_set_min_width (column, 35);
	gtk_tree_view_column_set_alignment (column, 0.5);
	gtk_tree_view_append_column (bidding, column);

	gtk_widget_show_all (window_bidding);

	window_bidding_update (b);
}

void
window_bidding_delete (void)
{
	if (!window_bidding)
		return;

	gtk_widget_destroy (window_bidding);
	gtk_widget_destroy (store);
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
