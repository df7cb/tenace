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

static GtkWidget *window_play = 0;
static GtkTable *play_table;
static GtkLabel *play_label[7 * 13];

void window_play_update (board *b)
{
	if (!window_play)
		return;

	int t, i;
	for (t = 0; t < 13; t++) {
		for (i = 0; i < 7; i++) {
			GtkLabel *l = play_label[7*t + i];
			gtk_label_set_markup(l, "");
		}
		int h = 0; /* current column */
		for (i = 0; i < 4; i++) {
			card c = b->played_cards[4*t + i];
			if (c == -1)
				continue;
			if (!h)
				h = b->dealt_cards[c];
			GtkLabel *l = play_label[7*t + h++ - 1];
			if (c == claim_rest)
				gtk_label_set_markup(l, "CL");
			else {
				gtk_label_set_markup(l, card_string(c)->str);
				gtk_widget_set_sensitive(GTK_WIDGET(l), 4*t + i < b->n_played_cards);
			}
		}
	}
}

void window_play_init (board *b)
{
	if (window_play)
		return;

	window_play = create_window_play();
	//gtk_widget_show(window_play);
	play_table = GTK_TABLE(lookup_widget(window_play, "play_table"));
	assert (play_table);
	int cr, cc;
	for (cr = 0; cr <= 12; cr++) {
		char str[5];
		snprintf(str, 5, " %d ", cr+1);
		GtkWidget *lab = gtk_label_new(str);
		gtk_table_attach(play_table, lab, 0, 1, cr+1, cr+2, 0, 0, 0, 0);
		for (cc = 0; cc < 7; cc++) {
			GtkWidget *lab = gtk_label_new(NULL);
			gtk_table_attach(play_table, lab, cc+1, cc+2, cr+1, cr+2, 0, 0, 0, 0);
			play_label[7 * cr + cc] = GTK_LABEL(lab);
		}
	}
	gtk_widget_show_all(window_play);
	window_play_update (b);
}

void
window_play_delete (void)
{
	if (!window_play)
		return;

	gtk_widget_destroy (window_play);
	window_play = 0;
}
