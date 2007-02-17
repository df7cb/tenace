#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "interface.h"
#include "support.h"
#include "board.h"
#include "solve.h"

/* window_card */

GtkWidget *card_button[52];
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

/* /window_card */

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

