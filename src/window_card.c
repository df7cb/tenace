#include <assert.h>

#include "board.h"
#include "support.h"

static GtkWidget *card_button[52];
seat new_card_seat = west;

void cardX_clicked (GtkButton *button, gpointer cxp)
{
	suit cx = *(suit *)cxp;

	int i = 0;
	while (i < 13) {
		int c = cx * 13 + i;
		if (b->cards[c] == 0) {
			gtk_button_clicked((GtkButton*)card_button[c]);
			return;
		}
		i++;
	}
}

void card_clicked (GtkButton *button, gpointer cp)
{
	int dealt;
	card c = *(card *)cp;
	assert (new_card_seat >= 1 && new_card_seat <= 4);

	dealt = give_card(b, new_card_seat, c);
	gtk_button_set_relief (button, dealt ? GTK_RELIEF_NONE : GTK_RELIEF_NORMAL);
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

void card_window_update (seat *cards)
{
	int c;
	for (c = 0; c < 52; c++)
		gtk_button_set_relief ((GtkButton*)card_button[c],
			cards[c] ? GTK_RELIEF_NONE : GTK_RELIEF_NORMAL);
}

void deal_random(board *b)
{
	seat s;
	for (s = west; s <= south; s++) {
		while (b->hand_cards[s-1] < 13) {
			int c = rand() % 52;
			if (b->dealt_cards[c] == 0)
				give_card(b, s, c);
		}
	}
}
