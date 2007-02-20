#include <assert.h>
#include <stdlib.h>

#include "board.h"
#include "functions.h"
#include "support.h"

static GtkWidget *card_button[52];
seat new_card_seat = west;

void card_window_update (seat *cards)
{
	int c;
	for (c = 0; c < 52; c++)
		gtk_button_set_relief ((GtkButton*)card_button[c],
			cards[c] ? GTK_RELIEF_NONE : GTK_RELIEF_NORMAL);
}

void cardX_clicked (GtkButton *button, gpointer cxp)
{
	suit cx = *(suit *)cxp;

	if (b->hand_cards[new_card_seat-1] == 13) {
		board_statusbar(b->win, "Hand has already 13 cards");
		return;
	}

	int i;
	for (i = 0; i < 13; i++) {
		int c = cx * 13 + i;
		if (b->dealt_cards[c] == 0) {
			add_card(b, new_card_seat, c);

			board_statusbar(b->win, NULL);
			card_window_update(b->dealt_cards);
			show_board(b);
			return;
		}
	}
	board_statusbar(b->win, "All cards of that suit dealt");
}

void card_clicked (GtkButton *button, gpointer cp)
{
	card c = *(card *)cp;
	assert (new_card_seat >= 1 && new_card_seat <= 4);

	if (b->dealt_cards[c] && !b->cards[c]) {
		board_statusbar(b->win, "Card is in play and cannot be removed");
		return;
	}

	if (b->dealt_cards[c] == new_card_seat)
		remove_card(b, new_card_seat, c);
	else {
		if (b->hand_cards[new_card_seat-1] == 13) {
			board_statusbar(b->win, "Hand has already 13 cards");
			return;
		}

		if (b->dealt_cards[c])
			remove_card(b, b->dealt_cards[c], c);
		add_card(b, new_card_seat, c);
	}

	board_statusbar(b->win, NULL);
	card_window_update(b->dealt_cards);
	show_board(b);
}

void fill_card_window (GtkWidget *w)
{
	GtkTable *table = (GtkTable*) lookup_widget(w, "table_cards");
	static card c[52];
	static card cx[4];
	card cr, cc;
	int i = 51;
	for (cr = 0; cr <= 3; cr++) {
		for (cc = 0; cc <= 12; cc++) {
			GtkWidget *b = gtk_button_new_with_label(rank_string(12-cc));
			c[i] = i;
			g_signal_connect (b, "clicked", G_CALLBACK(card_clicked), (gpointer)(&c[i]));
			gtk_button_set_focus_on_click ((GtkButton*)b, 0);
			gtk_table_attach(table, b, cc+1, cc+2, cr, cr+1, 0, 0, 0, 0);
			gtk_widget_show(b);
			card_button[i] = b;
			i--;
		}
	}
	for (cr = 0; cr <= 3; cr++) {
		GtkWidget *b = gtk_button_new_with_label("x");
		cx[cr] = 3 - cr;
		g_signal_connect (b, "clicked", G_CALLBACK(cardX_clicked), (gpointer)(&cx[cr]));
		gtk_button_set_focus_on_click ((GtkButton*)b, 0);
		gtk_table_attach(table, b, 14, 15, cr, cr+1, 0, 0, 0, 0);
		gtk_widget_show(b);
	}
}

