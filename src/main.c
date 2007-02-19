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
#include "window_card.h"

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

  window_hand = create_window_hand ();
  gtk_widget_show (window_hand);
  window_card = create_window_card ();
  gtk_widget_show (window_card);
  //window_bid = create_window_bid ();
  //gtk_widget_show (window_bid);
  //window_bids = create_window_bids ();
  //gtk_widget_show (window_bids);
  gtk_widget_show (create_window_line_entry());

  srand(time(NULL));

  create_card_buttons();
  fill_card_window(window_card);

  b = board_new();
  b->win = window_hand;
  show_board(b);

  gtk_main ();
  return 0;
}

