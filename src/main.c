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
#include "window_play.h"

int
main (int argc, char *argv[])
{
  //GtkWidget *window_bid;
  GtkWidget *window_card;
  //GtkWidget *window_bids;

#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  add_pixmap_directory (PACKAGE_DATA_DIR "/" PACKAGE "/pixmaps");

  init_solve();
  srand(time(NULL));

  win = create_board_window ();

  b = win->boards[0]; //FIXME: remove this

  window_card = create_window_card ();
  gtk_widget_show (window_card);
  //window_bid = create_window_bid ();
  //gtk_widget_show (window_bid);
  //window_bids = create_window_bids ();
  //gtk_widget_show (window_bids);
  //gtk_widget_show (create_window_line_entry());

  window_play_init ();

  fill_card_window(window_card);

  if (argc > 1) {
	if (!board_load(argv[1], b))
		printf ("open failed.\n");
	card_window_update(b->dealt_cards);
	show_board(b);
  }

  show_board(b);

  gtk_main ();
  return 0;
}

