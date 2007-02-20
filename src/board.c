#include <ctype.h>
#include <stdio.h>

#include "bridge.h"
#include "functions.h"
#include "solve.h"
#include "support.h"
#include "window_line_entry.h"
#include "window_play.h"

board *b; /* currently visible board */

static GtkWidget *card_button[52];
static GtkWidget *card_button_child[52];
static GtkWidget *card_button_container[52]; /* non-NULL if button is shown */

void show_board (board *b)
{
	GtkWidget *win = b->win;
	GtkWidget *w;
	GString *str;

	w = lookup_widget(win, "label_board");
	str = g_string_new(b->name->str);
	g_string_append_printf(str, "\n%s",
		contract_string(b->level, b->trumps, b->declarer, b->doubled));
	gtk_label_set_text((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_west");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[1] ? "red" : "green",
		b->current_turn == west ? " weight=\"bold\"" : "",
		b->hand_name[0]->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_north");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[0] ? "red" : "green",
		b->current_turn == north ? " weight=\"bold\"" : "",
		b->hand_name[1]->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_east");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[1] ? "red" : "green",
		b->current_turn == east ? " weight=\"bold\"" : "",
		b->hand_name[2]->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_south");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[0] ? "red" : "green",
		b->current_turn == south ? " weight=\"bold\"" : "",
		b->hand_name[3]->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);

	w = lookup_widget(win, "label_tricks");
	g_string_printf(str, "NS: %d\nEW: %d", b->tricks[0], b->tricks[1]);
	gtk_label_set_markup((GtkLabel*) w, str->str);

	/*
	int label_i;
	for (label_i = 0; label_i < 52 && b->card_label[label_i]; label_i++) {
		gtk_widget_destroy(b->card_label[label_i]);
		b->card_label[label_i] = 0;
	}
	label_i = 0;
	*/

	int c;
	GtkWidget *box;

	char *box_array[4][4] = {{"hbox_west_c", "hbox_west_d", "hbox_west_h", "hbox_west_s"},
		{"hbox_north_c", "hbox_north_d", "hbox_north_h", "hbox_north_s"},
		{"hbox_east_c", "hbox_east_d", "hbox_east_h", "hbox_east_s"},
		{"hbox_south_c", "hbox_south_d", "hbox_south_h", "hbox_south_s"}};

	for (c = 51; c >= 0; c--) {
		int h = b->cards[c];
		int s = SUIT(c);

		GtkWidget *lab = card_button[c];
		if (card_button_container[c]) {
			gtk_container_remove(GTK_CONTAINER(card_button_container[c]), lab);
			card_button_container[c] = NULL;
		}
		if (h) {
			box = lookup_widget(win, box_array[h-1][s]);
			gtk_box_pack_start (GTK_BOX (box), lab, FALSE, FALSE, FALSE);
			gtk_widget_show(lab);
			card_button_container[c] = box;
		}
	}

	gtk_widget_show_all(win);

	char *labels[] = {0, "card_west", "card_north", "card_east", "card_south"};
	int i;
	for (i = west; i <= south; i++) {
		GtkWidget *label = lookup_widget(b->win, labels[i]);
		gtk_label_set_text(GTK_LABEL(label), "");
	}
	if (b->n_played_cards) {
		int trick_start = b->n_played_cards - seat_mod(b->n_played_cards);
		for (i = trick_start; i < b->n_played_cards; i++) {
			card c = b->played_cards[i];
			seat s = b->dealt_cards[c];
			g_string_printf(str, "<span%s>%s</span>",
				i == trick_start ? " underline=\"low\"" : "",
				card_string(c)->str);
			GtkWidget *label = lookup_widget(b->win, labels[s]);
			gtk_label_set_markup(GTK_LABEL(label), str->str);
		}
	}

	g_string_free(str, TRUE);

	if (b->par_score == -1) {
		w = lookup_widget(b->win, "par_label");
		gtk_label_set_text(GTK_LABEL(w), "");
	}

	if (run_dd)
		hilight_dd_scores(b);

	window_play_update(b);
	for (i = 0; i < b->n_bids; i++)
		printf ("%s\n", bid_string(b->bidding[i])->str);
	line_entry_set_from_board(b);
}

void button_set_markup(card c, char *text)
{
	gtk_label_set_markup(GTK_LABEL(card_button_child[c]), text);
}

void button_clear_markups()
{
	int c;
	for (c = 0; c < 52; c++) {
		if (card_button_container[c]) {
			button_set_markup(c, rank_string(RANK(c)));
		}
	}
}

static void button_clicked(GtkButton *l, card *cp)
{
	printf("Clicked: %s.\n", card_string(*cp)->str);
	if (play_card(b, b->cards[*cp], *cp))
		show_board(b);
}

static void button_entered(GtkButton *l, card *cp)
{
	char buf[100];

	board_statusbar(b->win, NULL);

	if (b->card_score[*cp] < 0)
		return;

	snprintf(buf, 99, "%s: %s",
		card_string(*cp)->str,
		score_string(b->level, b->trumps, b->declarer, b->doubled, b->vuln[b->declarer % 2],
			b->card_score[*cp], b->current_turn));
	board_statusbar(b->win, buf);
}

static void button_left(GtkButton *l, card *cp)
{
	board_statusbar(b->win, NULL);
}

void create_card_buttons ()
{
	static card card_data[52];
	card c;
	for (c = 0; c < 52; c++) {
		GtkWidget *lab = gtk_label_new(rank_string(RANK(c)));
		gtk_label_set_use_markup(GTK_LABEL(lab), TRUE);
		GtkWidget *but = gtk_button_new();
		gtk_container_add(GTK_CONTAINER(but), lab);
		//gtk_container_set_border_width(GTK_CONTAINER(but), 0);
		gtk_button_set_focus_on_click(GTK_BUTTON(but), FALSE);

		card_data[c] = c;
		g_signal_connect (but, "clicked", G_CALLBACK(button_clicked), &card_data[c]);
		g_signal_connect (but, "enter", G_CALLBACK(button_entered), &card_data[c]);
		g_signal_connect (but, "leave", G_CALLBACK(button_left), &card_data[c]);
		card_button[c] = but;
		card_button_child[c] = lab;
		card_button_container[c] = NULL;
		g_object_ref(but); // create reference so buttons are not deleted when moved around
	}
}


