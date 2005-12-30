#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <assert.h>
#include "interface.h"
#include "support.h"
#include "bridge.h"

board *b;
seat new_card_seat = west;
GtkWidget *card_button[52];

void show_board (board *b)
{
	GtkWidget *win = b->win;
	GtkWidget *w;
	w = lookup_widget(win, "label_board");
	gtk_label_set_text((GtkLabel*) w, b->name->str);
	w = lookup_widget(win, "text_west");
	gtk_label_set_text((GtkLabel*) w, hand_string(b->hands[0])->str);
	w = lookup_widget(win, "text_north");
	gtk_label_set_text((GtkLabel*) w, hand_string(b->hands[1])->str);
	w = lookup_widget(win, "text_east");
	gtk_label_set_text((GtkLabel*) w, hand_string(b->hands[2])->str);
	w = lookup_widget(win, "text_south");
	gtk_label_set_text((GtkLabel*) w, hand_string(b->hands[3])->str);
}

void card_clicked (GtkButton *button, gpointer cp)
{
	int dealt;
	card c = *(card *)cp;
	suit s = SUIT(c);
	rank r = RANK(c);
	assert (s && new_card_seat >= 1 && new_card_seat <= 4);

	if (r == cardX) {
		int i = 12;
		while (i >= 0) {
			if (b->cards[(4 - s) * 13 + i] == 0) {
				gtk_button_clicked((GtkButton*)card_button[(4 - s) * 14 + i]);
				return;
			}
			i--;
		}
		return;
	}

	dealt = give_card(b, new_card_seat, c);
	gtk_button_set_relief (button, dealt ? GTK_RELIEF_NONE : GTK_RELIEF_NORMAL);
	show_board(b);
}

static void fill_card_window (GtkWidget *w)
{
	GtkTable *table = (GtkTable*) lookup_widget(w, "table_cards");
	static card c[56];
	card cr, cc;
	int i = 0;
	for (cr = 0; cr <= 3; cr++) {
		for (cc = 0; cc <= 13; cc++) {
			GtkWidget *b = gtk_button_new_with_label(rank_string(12-cc));
			c[i] = ((4-cr)<<4) | (cc == 13 ? 13 : 12-cc);
			g_signal_connect (b, "clicked", G_CALLBACK(card_clicked), (gpointer)(&c[i]));
			gtk_button_set_focus_on_click ((GtkButton*)b, 0);
			gtk_table_attach(table, b, cc+1, cc+2, cr, cr+1, 0, 0, 0, 0);
			gtk_widget_show(b);
			card_button[i] = cc != 13 ? b : NULL;
			i++;
		}
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

  /*
   * The following code was added by Glade to create one of each component
   * (except popup menus), just so that you see something after building
   * the project. Delete any components that you don't want shown initially.
   */
  window_hand = create_window_hand ();
  gtk_widget_show (window_hand);
  window_bid = create_window_bid ();
  gtk_widget_show (window_bid);
  window_card = create_window_card ();
  gtk_widget_show (window_card);
  window_bids = create_window_bids ();
  gtk_widget_show (window_bids);

  fill_card_window(window_card);

  b = board_new();
  b->win = window_hand;
  show_board(b);

  gtk_main ();
  return 0;
}

