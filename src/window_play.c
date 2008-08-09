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
#include "window_board.h"

static GtkWidget *window_play = 0;
static GtkTable *play_table;
static GtkLabel *play_label[7 * 13];

void window_play_update (board *b)
{
	if (!window_play)
		return;

	int t, i;
	for (t = 0; t < 13; t++) { /* trick */
		for (i = 0; i < 7; i++) {
			GtkLabel *l = play_label[7*t + i];
			gtk_label_set_markup(l, "");
		}
		int col = -1; /* current column */
		for (i = 0; i < 4; i++) { /* card in trick */
			card c = b->played_cards[4*t + i];
			if (c == -1)
				continue;
			if (col < 0)
				col = c == claim_rest ? 0 : b->dealt_cards[c] - 1;
			GtkLabel *l = play_label[7*t + col++];
			if (c == claim_rest)
				gtk_label_set_markup(l, _("CL"));
			else {
				gtk_label_set_markup(l, card_string_color (c));
			}
			gtk_widget_set_sensitive(GTK_WIDGET(l), 4*t + i < b->n_played_cards);
		}
	}
}

void window_play_init (window_board_t *win, board *b)
{
	if (window_play)
		return;

	window_play = glade_xml_get_widget (win->xml, "window_play");
	play_table = GTK_TABLE(glade_xml_get_widget (win->xml, "play_table"));
	assert (play_table);
	int cr, cc;
	char str[20];
	for (cc = 1; cc < 8; cc++) {
		GtkWidget *lab = gtk_label_new (NULL);
		snprintf (str, sizeof (str), "<b>%s</b>", _(seat_str[seat_mod (cc)]));
		gtk_label_set_markup (GTK_LABEL (lab), str);
		gtk_table_attach(play_table, lab, cc, cc+1, 0, 1, 0, 0, 0, 0);
	}
	for (cr = 0; cr < 13; cr++) {
		snprintf(str, sizeof (str), " %d ", cr+1);
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

	gtk_widget_hide (window_play);
	window_play = 0;
}

gboolean
on_window_play_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	GtkCheckMenuItem *menuitem = GTK_CHECK_MENU_ITEM (glade_xml_get_widget (win->xml, "play1"));
	gtk_check_menu_item_set_active (menuitem, FALSE);
	window_play_delete ();
	return FALSE;
}

void
on_play1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (gtk_check_menu_item_get_active (GTK_CHECK_MENU_ITEM (menuitem)))
		window_play_init (win, CUR_BOARD);
	else
		window_play_delete ();
}
