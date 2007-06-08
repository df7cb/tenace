#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <gtk/gtk.h>

#include "../handdisplay/hand_display.h"

#include "window_board.h"

#include "functions.h"
#include "interface.h"
#include "solve.h"
#include "support.h"
#include "window_line_entry.h"
#include "window_play.h"

window_board_t *win; // FIXME static?

//static GtkWidget *card_button[52];
//static GtkWidget *card_button_child[52];
//static GtkWidget *card_button_container[52]; /* non-NULL if button is shown */

void show_board (board *b, redraw_t redraw)
{
	GtkWidget *w;
	GString *str = g_string_new(NULL);

	if (redraw & REDRAW_TITLE) {
		char *fname = b->filename ? strrchr(b->filename->str, '/') + 1 : "";
		g_string_printf(str, "Tenace - %s%s%s", b->name->str,
			b->filename ? " - " : "", fname ? fname : "");
		gtk_window_set_title(GTK_WINDOW(win->window), str->str);

		w = lookup_widget(win->window, "label_board");
		g_string_printf(str, "%s\n%s", b->name->str,
			contract_string(b->level, b->trumps, b->declarer, b->doubled));
		gtk_label_set_text((GtkLabel*) w, str->str);
	}

	if (redraw & REDRAW_NAMES) {
		w = lookup_widget(win->window, "label_west");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[1] ? "red" : "green",
			b->current_turn == west ? " weight=\"bold\"" : "",
			b->hand_name[0]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);

		w = lookup_widget(win->window, "label_north");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[0] ? "red" : "green",
			b->current_turn == north ? " weight=\"bold\"" : "",
			b->hand_name[1]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);

		w = lookup_widget(win->window, "label_east");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[1] ? "red" : "green",
			b->current_turn == east ? " weight=\"bold\"" : "",
			b->hand_name[2]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);

		w = lookup_widget(win->window, "label_south");
		g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
			b->vuln[0] ? "red" : "green",
			b->current_turn == south ? " weight=\"bold\"" : "",
			b->hand_name[3]->str);
		gtk_label_set_markup((GtkLabel*) w, str->str);
	}

	if (redraw & REDRAW_TITLE) {
		w = lookup_widget(win->window, "label_tricks");
		g_string_printf(str, "NS: %d\nEW: %d", b->tricks[0], b->tricks[1]);
		gtk_label_set_markup((GtkLabel*) w, str->str);
	}

	/*
	int label_i;
	for (label_i = 0; label_i < 52 && b->card_label[label_i]; label_i++) {
		gtk_widget_destroy(b->card_label[label_i]);
		b->card_label[label_i] = 0;
	}
	label_i = 0;
	*/

	if (redraw & REDRAW_DD) {
		if (b->par_score == -1) {
			w = lookup_widget(win->window, "par_label");
			gtk_label_set_text(GTK_LABEL(w), "");
		}

		if (run_dd)
			hilight_dd_scores(b);
	}

	if (redraw & REDRAW_HANDS) {
		int i, c;
		for (i = west; i <= south; i++) {
			for (c = 51; c >= 0; c--) {
				seat h = b->cards[c];
				hand_display_set_card (win->handdisp[i - 1], c, h == i);
				if (h == i && b->card_score[c] >= 0)
					hand_display_set_card_score (win->handdisp[i - 1], c,
						card_overtricks(b, c));
			}
			hand_display_set_card_score_neg (win->handdisp[i - 1],
					i % 2 != b->declarer % 2);
			hand_display_draw(GTK_WIDGET (win->handdisp[i - 1]));
		}

		char *labels[] = {0, "card_west", "card_north", "card_east", "card_south"};
		for (i = west; i <= south; i++) {
			GtkWidget *label = lookup_widget(win->window, labels[i]);
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
				GtkWidget *label = lookup_widget(win->window, labels[s]);
				gtk_label_set_markup(GTK_LABEL(label), str->str);
			}
		}

		line_entry_set_from_board(b);
	}

	g_string_free(str, TRUE);

	if (redraw & REDRAW_PLAY)
		window_play_update(b);

	if (redraw & REDRAW_BIDDING) {
		int i;
		for (i = 0; i < b->n_bids; i++)
			printf ("%s\n", bid_string(b->bidding[i])->str);
	}
}

static void card_clicked (HandDisplay *handdisp, int *cp, int *seatp)
{
	board *b = CUR_BOARD;
	printf("Clicked: %s for %c.\n", card_string(*cp)->str, "WNES"[*seatp - 1]);
	if (play_card(b, b->cards[*cp], *cp))
		show_board(b, REDRAW_HANDS | REDRAW_TRICKS);
}

static void card_enter (HandDisplay *handdisp, int *cp, int *seatp)
{
	char buf[100];

	board_statusbar(NULL);

	board *b = CUR_BOARD;
	if (b->card_score[*cp] < 0)
		return;

	snprintf(buf, 99, "%s: %s",
		card_string(*cp)->str,
		score_string(b->level, b->trumps, b->declarer, b->doubled, b->vuln[b->declarer % 2],
			b->card_score[*cp], b->current_turn));
	board_statusbar(buf);
}

static void card_leave (HandDisplay *handdisp, int *cp, int *seatp)
{
	board_statusbar(NULL);
}

static void create_hand_widgets (window_board_t *win)
{
	static const char *alignment_a[] = {"alignment_w", "alignment_n", "alignment_e", "alignment_s"};
	static int dir[] = { 1, 2, 3, 4 };
	int h;

	for (h = 0; h < 4; h++) {
		GtkWidget *alignment = lookup_widget(win->window, alignment_a[h]);
		GtkWidget *hand = hand_display_new();
		gtk_container_add(GTK_CONTAINER(alignment), hand);
		gtk_widget_show(hand);
		g_signal_connect (hand, "card-clicked", G_CALLBACK (card_clicked), dir + h);
		g_signal_connect (hand, "card-enter", G_CALLBACK (card_enter), dir + h);
		g_signal_connect (hand, "card-leave", G_CALLBACK (card_leave), dir + h);
		win->handdisp[h] = HAND_DISPLAY(hand);
		//gtk_widget_show_all(handdisp);
	}

		/*
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
		win->card_button[c] = but;
		win->card_button_child[c] = lab;
		win->card_button_container[c] = NULL;
		g_object_ref(but); // create reference so buttons are not deleted when moved around
	}
	*/
}

void
board_window_append_board (board *b)
{
	if (win->n_boards >= win->n_boards_alloc) {
		win->n_boards_alloc <<= 2;
		win->boards = realloc(win->boards, win->n_boards_alloc);
		assert(win->boards);
	}
	win->boards[win->n_boards++] = b;

	GtkWidget *m = gtk_menu_item_new_with_label (b->name->str);
	gtk_menu_append ((win->board_menu), m);
}

void
board_window_init ()
{
	win = malloc(sizeof(window_board_t));
	win->window = create_window_hand ();

	//create_card_buttons(win);
	create_hand_widgets(win);

	win->boards = malloc(4 * sizeof(board*));
	assert (win->boards);
	win->n_boards_alloc = 4;
	win->n_boards = 0;

	board_window_append_board (board_new ());
	win->cur = 0;

	win->statusbar = GTK_STATUSBAR (lookup_widget(win->window, "statusbar1"));
	win->board_menu = GTK_MENU (lookup_widget(win->window, "board_menu1_menu"));

	gtk_widget_show (win->window);
}

void board_statusbar (char *text)
{
	static guint id = 0;
	if (!id)
		id = gtk_statusbar_get_context_id(win->statusbar, "window_board_c");

	gtk_statusbar_pop(win->statusbar, id);
	if (text)
		gtk_statusbar_push(win->statusbar, id, text);
}

void
board_set_dealer (seat dealer)
{
	board *b = CUR_BOARD;
	board_rewind(b);
	b->declarer = dealer;
	b->current_turn = seat_mod(dealer + 1);
	show_board(b, REDRAW_CONTRACT | REDRAW_TRICKS | REDRAW_HANDS);
}

void
board_set_trumps (suit trumps)
{
	board *b = CUR_BOARD;
	b->trumps = trumps;
	show_board(b, REDRAW_CONTRACT);
}

void
board_set_level (int level)
{
	board *b = CUR_BOARD;
	b->level = level;
	calculate_target(b);
	show_board(b, REDRAW_CONTRACT);
}

void
board_set_vuln (int ns, int ew)
{
	board *b = CUR_BOARD;
	b->vuln[0] = ns;
	b->vuln[1] = ew;
	b->par_score = -1;
	show_board(b, REDRAW_CONTRACT);
}

void
board_toggle_doubled (int button)
{
	static int double_update_in_progress = 0;
	static GtkCheckMenuItem *x = 0, *xx = 0;

	if (double_update_in_progress++)
		return;

	if (!x)
		x = GTK_CHECK_MENU_ITEM(lookup_widget(win->window, "level_doubled1"));
	if (!xx)
		xx = GTK_CHECK_MENU_ITEM(lookup_widget(win->window, "level_redoubled1"));

	board *b = CUR_BOARD;

	switch (button) {
	case 1:
		if (gtk_check_menu_item_get_active(x)) {
			b->doubled = 1;
			gtk_check_menu_item_set_active(xx, FALSE);
		} else
			b->doubled = 0;
		break;
	case 2:
		if (gtk_check_menu_item_get_active(xx)) {
			b->doubled = 2;
			gtk_check_menu_item_set_active(x, FALSE);
		} else
			b->doubled = 0;
		break;
	default:
		assert(0);
	}

	show_board(b, REDRAW_CONTRACT);
	double_update_in_progress = 0;
}
