#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "interface.h"
#include "support.h"
#include "bridge.h"
#include "solve.h"

board *b;
seat new_card_seat = west;
GtkWidget *card_button[52];
GtkWidget *card_label[52];
GtkWidget *card_label_child[52];
GtkWidget *card_label_container[52];

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
	g_string_printf(str, "%s%s%s",
		b->current_lead == west ? "<b>" : "",
		b->hands[0]->name->str,
		b->current_lead == west ? "</b>" : "");
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_north");
	g_string_printf(str, "%s%s%s",
		b->current_lead == north ? "<b>" : "",
		b->hands[1]->name->str,
		b->current_lead == north ? "</b>" : "");
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_east");
	g_string_printf(str, "%s%s%s",
		b->current_lead == east ? "<b>" : "",
		b->hands[2]->name->str,
		b->current_lead == east ? "</b>" : "");
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_south");
	g_string_printf(str, "%s%s%s",
		b->current_lead == south ? "<b>" : "",
		b->hands[3]->name->str,
		b->current_lead == south ? "</b>" : "");
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

		GtkWidget *lab = card_label[c];
		if (card_label_container[c]) {
			gtk_container_remove(GTK_CONTAINER(card_label_container[c]), lab);
			card_label_container[c] = NULL;
		}
		if (h) {
			box = lookup_widget(win, box_array[h-1][s]);
			gtk_box_pack_start (GTK_BOX (box), lab, FALSE, FALSE, FALSE);
			gtk_widget_show(lab);
			card_label_container[c] = box;
		}
	}
	gtk_widget_show_all(win);
}

void label_set_markup(card c, char *text)
{
	//gtk_label_set_markup(GTK_LABEL(card_label[c]), text);
	gtk_label_set_markup(GTK_LABEL(card_label_child[c]), text);
}

void label_clear_markups()
{
	int c;
	for (c = 0; c < 52; c++) {
		if (card_label_container[c]) {
			char buf[3];
			snprintf(buf, 2, "%s", rank_string(RANK(c)));
			label_set_markup(c, buf);
		}
	}
}

void label_clicked(GtkLabel *l, void *foo, card *cp)
{
	printf("Clicked: %s.\n", card_string(*cp)->str);
	//gtk_label_set_markup(l, "<span background=\"red\">J</span>");
	if (play_card(b, b->cards[*cp], *cp))
		show_board(b);
}

GtkStatusbar *statusbar;
static guint statusbar_id = 0;

void label_entered(GtkLabel *l, card *cp)
{
	char buf[100];

	if (b->card_score[*cp] < 0)
		return;

	//GtkStatusbar *statusbar;
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	//static guint id = 0;
	if (!statusbar_id)
		statusbar_id = gtk_statusbar_get_context_id(statusbar, "mouseover");

	snprintf(buf, 99, "%s: %s %s",
		card_string(*cp)->str,
		contract_string(b->level, b->trumps, b->declarer, b->doubled),
		overtricks(7 - b->level - b->card_score[*cp]));
	gtk_statusbar_push(statusbar, statusbar_id, buf);
}

void label_left(GtkLabel *l, card *cp)
{
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	if (!statusbar_id)
		statusbar_id = gtk_statusbar_get_context_id(statusbar, "mouseover");

	gtk_statusbar_pop(statusbar, statusbar_id);
}

static void create_card_labels ()
{
	static card cards[52];
	card c;
	for (c = 0; c < 52; c++) {
		//GtkWidget *wn = gtk_label_new(rank_string(RANK(c)));
		GtkWidget *lab = gtk_label_new(rank_string(RANK(c)));
		gtk_label_set_use_markup(GTK_LABEL(lab), TRUE);
		GtkWidget *wn = gtk_button_new();
		gtk_container_add(GTK_CONTAINER(wn), lab);
		//gtk_label_set_selectable (GTK_LABEL (wn), TRUE);
		gtk_container_set_border_width(GTK_CONTAINER(wn), 0);
		gtk_button_set_focus_on_click(GTK_BUTTON(wn), FALSE);
		cards[c] = c;
		g_signal_connect (wn, "button-press-event", G_CALLBACK(label_clicked), &cards[c]);
		g_signal_connect (wn, "enter", G_CALLBACK(label_entered), &cards[c]);
		g_signal_connect (wn, "leave", G_CALLBACK(label_left), &cards[c]);
		card_label[c] = wn;
		card_label_child[c] = lab;
		card_label_container[c] = NULL;
		g_object_ref(wn); // create reference so labels are not deleted when moved around
	}
}

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

static void fill_card_window (GtkWidget *w)
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

int
main (int argc, char *argv[])
{
  GtkWidget *window_hand;
  GtkWidget *window_bid;
  GtkWidget *window_card;
  GtkWidget *window_bids;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  init_solve();

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  window_hand = create_window_hand ();
  gtk_widget_show (window_hand);
  window_bid = create_window_bid ();
  //gtk_widget_show (window_bid);
  window_card = create_window_card ();
  gtk_widget_show (window_card);
  window_bids = create_window_bids ();
  //gtk_widget_show (window_bids);

  srand(time(NULL));

  create_card_labels();
  fill_card_window(window_card);

  b = board_new();
  b->win = window_hand;
  show_board(b);

  gtk_main ();
  return 0;
}

