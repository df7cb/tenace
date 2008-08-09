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
#include <stdlib.h>

#include "bridge.h"
#include "functions.h"
#include "window_board.h" /* board b */

#include "window_card.h" /* board b */

static GtkWidget *window_card = 0;
static HandDisplay *hand_display = 0;
static int seat_null = 0;
seat new_card_seat = west;

void
card_window_update (seat *cards)
{
	if (!window_card)
		return;
	static seat *cards_ptr = NULL;
	if (cards)
		cards_ptr = cards;
	if (!cards_ptr)
		return;

	int c;
	for (c = 0; c < 52; c++)
		hand_display_set_card (hand_display, c,
			cards_ptr[c] ?
			( cards_ptr[c] == new_card_seat ?
			  HAND_DISPLAY_HILIGHT_CARD : HAND_DISPLAY_GREY_CARD )
			: HAND_DISPLAY_CARD);
	hand_display_draw (GTK_WIDGET (hand_display));
}

void
set_new_card_seat (seat s)
{
	new_card_seat = s;
	card_window_update (NULL);
}

static void
card_clicked (HandDisplay *handdisp, int c, int *seatp)
{
	if (c == -1)
		return;

	assert (c >= 0 && c < 56);
	assert (new_card_seat >= 1 && new_card_seat <= 4);
	board *b = win->boards[win->cur];

	/* x clicked */
	if (c >= 52) {
		if (b->hand_cards[new_card_seat-1] == 13) {
			board_statusbar(_("Hand has already 13 cards"));
			return;
		}

		int i;
		for (i = 0; i < 13; i++) {
			int new = (c - 52) * 13 + i;
			if (b->dealt_cards[new] == 0) {
				add_card(b, new_card_seat, new);
				b->par_score = -1;

				board_statusbar(NULL);
				card_window_update(b->dealt_cards);
				hand_display_draw(GTK_WIDGET (hand_display)); /* force redraw */
				show_board(b, REDRAW_HANDS | REDRAW_PAR);
				return;
			}
		}
		board_statusbar(_("All cards of that suit dealt"));
		return;
	}

	if (b->dealt_cards[c] && !b->cards[c]) {
		board_statusbar(_("Card is in play and cannot be removed"));
		return;
	}

	if (b->dealt_cards[c] == new_card_seat)
		remove_card(b, new_card_seat, c);
	else {
		if (b->hand_cards[new_card_seat-1] == 13) {
			board_statusbar(_("Hand has already 13 cards"));
			return;
		}

		if (b->dealt_cards[c])
			remove_card(b, b->dealt_cards[c], c);
		add_card(b, new_card_seat, c);
	}
	b->par_score = -1;

	board_statusbar(NULL);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS | REDRAW_PAR);
}

static void
window_card_drag_drop (HandDisplay *handdisp, int c, int on_card, int *seatp /* unused, always 0 here */)
{
	printf ("dropped %d on %d on card window\n", c, on_card);
	if (c == -1)
		return;

	board *b = win->boards[win->cur];

	if (b->dealt_cards[c] && !b->cards[c]) {
		board_statusbar(_("Card is in play and cannot be removed"));
		return;
	}

	seat from_seat = b->dealt_cards[c];
	if (! from_seat)
		return;
	remove_card(b, from_seat, c);

	if (on_card >= 0 && b->dealt_cards[on_card] == 0)
		add_card (b, from_seat, on_card);

	b->par_score = -1;

	board_statusbar (NULL);
	card_window_update (b->dealt_cards);
	show_board (b, REDRAW_HANDS | REDRAW_PAR);
}

void
window_card_set_style (int style)
{
	/* doesn't actually set the style, but makes the right drag icon appear */
	if (window_card)
		hand_display_set_style(hand_display, style);
}

void
window_card_init (int style)
{
	if (window_card)
		return;

	GtkWidget *w = hand_display_new (HAND_DISPLAY_MODE_HAND_X);
	hand_display = HAND_DISPLAY (w);
	int c;
	for (c = 0; c < 52; c++) {
		hand_display_set_card (hand_display, c, HAND_DISPLAY_CARD);
	}
	g_signal_connect (hand_display, "card-clicked", G_CALLBACK (card_clicked), NULL);
	g_signal_connect (hand_display,
			"card-drag-drop", G_CALLBACK (window_card_drag_drop), &seat_null);
	hand_display_set_drag (hand_display, 1);

	window_card = glade_xml_get_widget (win->xml, "window_card");
	GtkWidget *vbox = glade_xml_get_widget (win->xml, "vbox2");
	gtk_widget_show (w);
	gtk_box_pack_start_defaults (GTK_BOX (vbox), w);
	gtk_widget_show (window_card);
	card_window_update(CUR_BOARD->dealt_cards);

	window_card_set_style (style);
}

void
window_card_delete (void)
{
	if (!window_card)
		return;

	gtk_widget_hide (window_card);
	window_card = NULL;
}
