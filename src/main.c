/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2008 Christoph Berg <cb@df7cb.de>
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

#include "bridge.h"
#include "file.h"
#include "functions.h"
#include "options.h"
#include "solve.h"
#include "window_board.h"
#include "window_card.h"
#include "window_play.h"

int
main (int argc, char *argv[])
{
#ifdef ENABLE_NLS
  bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);
#endif

  gtk_set_locale ();
  gtk_init (&argc, &argv);

  init_solve();
  srand(time(NULL));

  win = malloc(sizeof(window_board_t));
  win->xml = glade_xml_new (PACKAGE_DATA_DIR "/" PACKAGE "/" PACKAGE ".glade", NULL, NULL);
  glade_xml_signal_autoconnect (win->xml);

  board_window_init (win);
  read_config (win);

  if (argc > 1) {
	char *fname = argv[1];
	if (! g_path_is_absolute (argv[1])) {
		char *cwd = g_get_current_dir ();
		fname = g_build_filename (cwd, argv[1], NULL);
		g_free (cwd);
	}
	if (! board_load (win, fname)) {
		if (errno == EMEDIUMTYPE) {
			perror (fname);
			puts (_("Hint: tenace can only read files in .lin format"));
		} else
			perror (fname);
		exit (1);
	}
	recently_used_add (fname);
	if (! g_path_is_absolute (argv[1]))
		g_free (fname);
  } else {
	board_window_append_board (win, NULL);
  }

  show_board(win->boards[0], REDRAW_FULL);
  card_window_update(win->boards[0]->dealt_cards);

  gtk_main ();
  write_config (win);
  return 0;
}

