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
#include "interface.h"
#include "support.h"
#include "window_board.h" /* board b */

static GtkWidget *window_card = 0;
static HandDisplay *hand_display = 0;
seat new_card_seat = west;

void
card_window_update (seat *cards)
{
	if (!window_card)
		return;
	static *cards_ptr = NULL;
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
card_clicked (HandDisplay *handdisp, int *cp, int *seatp)
{
	card c = *cp;
	assert (c >= 0 && c < 56);
	assert (new_card_seat >= 1 && new_card_seat <= 4);
	board *b = win->boards[win->cur];

	/* x clicked */
	if (c >= 52) {
		if (b->hand_cards[new_card_seat-1] == 13) {
			board_statusbar("Hand has already 13 cards");
			return;
		}

		int i;
		for (i = 0; i < 13; i++) {
			int new = (c - 52) * 13 + i;
			if (b->dealt_cards[new] == 0) {
				add_card(b, new_card_seat, new);

				board_statusbar(NULL);
				card_window_update(b->dealt_cards);
				hand_display_draw(GTK_WIDGET (hand_display)); /* force redraw */
				show_board(b, REDRAW_HANDS);
				return;
			}
		}
		board_statusbar("All cards of that suit dealt");
		return;
	}

	if (b->dealt_cards[c] && !b->cards[c]) {
		board_statusbar("Card is in play and cannot be removed");
		return;
	}

	if (b->dealt_cards[c] == new_card_seat)
		remove_card(b, new_card_seat, c);
	else {
		if (b->hand_cards[new_card_seat-1] == 13) {
			board_statusbar("Hand has already 13 cards");
			return;
		}

		if (b->dealt_cards[c])
			remove_card(b, b->dealt_cards[c], c);
		add_card(b, new_card_seat, c);
	}

	board_statusbar(NULL);
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_HANDS);
}

void
window_card_init ()
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

	window_card = create_window_card ();
	GtkWidget *vbox = lookup_widget (window_card, "vbox2");
	gtk_widget_show (w);
	gtk_box_pack_start_defaults (vbox, w);
	gtk_widget_show (window_card);
}

void
window_card_delete (void)
{
	if (!window_card)
		return;

	gtk_widget_destroy (window_card);
	window_card = 0;
}
