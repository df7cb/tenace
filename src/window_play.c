#include <assert.h>

#include "bridge.h"
#include "functions.h"
#include "interface.h"
#include "support.h"
#include "window_board.h"

static GtkWidget *window_play = 0;
static GtkTable *play_table;
static GtkLabel *play_label[52];

void window_play_init ()
{
	if (window_play)
		return;

	window_play = create_window_play();
	gtk_widget_show(window_play);
	play_table = GTK_TABLE(lookup_widget(window_play, "play_table"));
	assert (play_table);
	int cr, cc;
	for (cr = 0; cr <= 12; cr++) {
		char str[5];
		snprintf(str, 5, " %d ", cr+1);
		GtkWidget *lab = gtk_label_new(str);
		gtk_table_attach(play_table, lab, 0, 1, cr, cr+1, 0, 0, 0, 0);
		for (cc = 0; cc <= 3; cc++) {
			GtkWidget *lab = gtk_label_new(NULL);
			gtk_table_attach(play_table, lab, cc+1, cc+2, cr, cr+1, 0, 0, 0, 0);
			play_label[4 * cr + cc] = GTK_LABEL(lab);
		}
	}
	gtk_widget_show_all(window_play);
}

void window_play_update (board *b)
{
	if (!window_play)
		return;

	int c;
	for (c = 0; c < 52; c++) {
		if (b->played_cards[c] == -1)
			gtk_label_set_markup(play_label[c], "");
		else if (b->played_cards[c] == claim_rest)
			gtk_label_set_markup(play_label[c], "CL");
		else {
			gtk_label_set_markup(play_label[c], card_string(b->played_cards[c])->str);
			gtk_widget_set_sensitive(GTK_WIDGET(play_label[c]), c < b->n_played_cards);
		}
	}
}

void
window_play_delete (void)
{
	if (!window_play)
		return;

	gtk_widget_destroy (window_play);
	window_play = 0;
}
