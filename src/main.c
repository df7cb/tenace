/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2007 Christoph Berg <cb@df7cb.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <time.h>

#include "interface.h"
#include "support.h"
#include "bridge.h"
#include "file.h"
#include "solve.h"
#include "window_board.h"
#include "window_card.h"
#include "window_play.h"

int
main (int argc, char *argv[])
{
  //GtkWidget *window_bid;
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

  //window_bid = create_window_bid ();
  //gtk_widget_show (window_bid);
  //GtkWidget *window_options = create_window_options ();
  //gtk_widget_show (window_options);
  //gtk_widget_show (create_window_line_entry());

  //window_play_init ();

  win = malloc(sizeof(window_board_t));

  board_window_init (win);
  read_config (win);

  if (argc > 1) {
	if (! board_load (win, argv[1])) {
		if (errno == EMEDIUMTYPE) {
			perror (argv[1]);
			puts (_("Hint: tenace can only read files in .lin format"));
		} else
			perror (argv[1]);
		exit (1);
	}
	card_window_update(win->boards[0]->dealt_cards);
  } else {
	board_window_append_board (win, NULL);
  }

  show_board(win->boards[0], REDRAW_FULL);

  gtk_main ();
  write_config (win);
  return 0;
}

