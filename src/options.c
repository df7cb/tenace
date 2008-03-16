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

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "support.h"
#include "window_board.h"
#include "window_card.h"

/* default paths */
static char *svg_files[] = {
	"/usr/share/gnome-games-common/cards/bonded.svg", /* lenny */
	"/usr/share/pixmaps/gnome-games-common/cards/bonded.svg", /* etch */
};

/* set options window contents from program data */
void
window_options_populate (GtkWidget *window_options)
{
	GtkWidget *w = lookup_widget (window_options, "show_played_cards");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), win->show_played_cards);

	w = lookup_widget (window_options,
		win->hand_display_style == HAND_DISPLAY_STYLE_CARDS ?
			"show_as_cards" : "show_as_text");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);

	w = lookup_widget (window_options, "svg_file");
	if (win->svg)
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (w), win->svg);

	w = lookup_widget (window_options, "spinbutton_card_width");
	gtk_spin_button_set_value (GTK_SPIN_BUTTON (w), win->card_width);

	switch (win->show_hands) {
		case seat_none: /* not yet implemented - is this useful? */
			w = lookup_widget (window_options, "show_hand_none");
			break;
		case east_west:
			w = lookup_widget (window_options, "show_hand_ew");
			break;
		case north_south:
			w = lookup_widget (window_options, "show_hand_ns");
			break;
		default:
			w = lookup_widget (window_options, "show_hand_all");
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);

	switch (win->show_dd_scores) {
		case seat_none: /* not yet implemented - is this useful? */
			w = lookup_widget (window_options, "show_dd_none");
			break;
		case east_west:
			w = lookup_widget (window_options, "show_dd_ew");
			break;
		case north_south:
			w = lookup_widget (window_options, "show_dd_ns");
			break;
		default:
			w = lookup_widget (window_options, "show_dd_all");
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);
}

/* set program data from options window */
void
apply_options (GtkWidget *window_options)
{
	GtkWidget *w = lookup_widget (window_options, "show_played_cards");
	win->show_played_cards = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w));

	w = lookup_widget (window_options, "show_as_cards");
	int style = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) ?
		HAND_DISPLAY_STYLE_CARDS : HAND_DISPLAY_STYLE_TEXT;
	board_window_set_style (win, style);
	window_card_set_style (style);

	w = lookup_widget (window_options, "svg_file");
	gchar *fname = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (w));
	w = lookup_widget (window_options, "spinbutton_card_width");
	win->card_width = gtk_spin_button_get_value (GTK_SPIN_BUTTON (w));

	if (fname) {
		if (win->svg)
			g_free (win->svg);
		win->svg = fname;
		hand_display_set_svg (win->svg, win->card_width);
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(lookup_widget (window_options, "show_dd_all"))))
	{
		win->show_dd_scores = seat_all;
	} else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(lookup_widget (window_options, "show_dd_ns"))))
	{
		win->show_dd_scores = north_south;
	} else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(lookup_widget (window_options, "show_dd_ew"))))
	{
		win->show_dd_scores = east_west;
	} else {
		win->show_dd_scores = seat_none;
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(lookup_widget (window_options, "show_hand_all"))))
	{
		win->show_hands = seat_all;
	} else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(lookup_widget (window_options, "show_hand_ns"))))
	{
		win->show_hands = north_south;
	} else {
		win->show_hands = east_west;
	}

	show_board(CUR_BOARD, REDRAW_HANDS);
}

/* read config from disk */
int
read_config (window_board_t *win)
{
	char rcfile[1024];
	snprintf (rcfile, sizeof (rcfile), "%s/%s",
		g_get_user_config_dir (), "/tenacerc");

	if (! g_key_file_load_from_file (win->keyfile, rcfile,
		G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, NULL) &&
	    ! g_key_file_load_from_data_dirs (win->keyfile, "tenacerc", NULL,
		G_KEY_FILE_KEEP_COMMENTS | G_KEY_FILE_KEEP_TRANSLATIONS, NULL)
	) {
		return 0;
	}

	/* Card display */

	char *p;
	if ((p = g_key_file_get_string (win->keyfile, "tenace", "show_as", NULL))) {
		win->hand_display_style = !strcmp (p, "cards") ?
			HAND_DISPLAY_STYLE_CARDS : HAND_DISPLAY_STYLE_TEXT;
	} else {
		win->hand_display_style = HAND_DISPLAY_STYLE_CARDS;
	}
	if (win->svg)
		g_free (win->svg);
	if ((p = g_key_file_get_string (win->keyfile, "tenace", "svg", NULL))) {
		win->svg = p;
	} else {
		win->svg = NULL;
	}
	int i;
	if ((i = g_key_file_get_integer (win->keyfile, "tenace", "card_width", NULL))) {
		win->card_width = i;
	}
	if ((i = g_key_file_get_boolean (win->keyfile, "tenace", "show_played_cards", NULL))) {
		win->show_played_cards = i;
	}

	/* check if the file is there */
	struct stat buf;
	if (win->svg) {
		if (stat (win->svg, &buf) == -1) {
			g_free (win->svg);
			win->svg = NULL;
		}
	}
	/* set a default */
	if (! win->svg)
		for (i = 0; i < sizeof (svg_files); i++) {
			if (stat (svg_files[i], &buf) != -1) {
				win->svg = strdup (svg_files[i]);
				break;
			}
		}

	/* Hands */

	if ((p = g_key_file_get_string (win->keyfile, "tenace", "show_hands", NULL))) {
		if (! strcmp (p, "none"))
			win->show_hands = seat_none;
		else if (! strcmp (p, "ns"))
			win->show_hands = north_south;
		else if (! strcmp (p, "ew"))
			win->show_hands = east_west;
		else
			win->show_hands = seat_all;
	}
	if ((p = g_key_file_get_string (win->keyfile, "tenace", "show_dd", NULL))) {
		if (! strcmp (p, "none"))
			win->show_dd_scores = seat_none;
		else if (! strcmp (p, "ns"))
			win->show_dd_scores = north_south;
		else if (! strcmp (p, "ew"))
			win->show_dd_scores = east_west;
		else
			win->show_dd_scores = seat_all;
	}

	board_window_set_style (win, win->hand_display_style);
	window_card_set_style (win->hand_display_style);
	if (win->hand_display_style == HAND_DISPLAY_STYLE_CARDS && win->svg) {
		hand_display_set_svg (win->svg, win->card_width);
	}

	return 1;
}

/* write config to disk */
int
write_config (window_board_t *win)
{
	/* Card display */
	g_key_file_set_string (win->keyfile, "tenace", "show_as",
		win->hand_display_style == HAND_DISPLAY_STYLE_CARDS ? "cards" : "text");
	if (win->svg)
		g_key_file_set_string (win->keyfile, "tenace", "svg", win->svg);
	g_key_file_set_integer (win->keyfile, "tenace", "card_width", win->card_width);
	g_key_file_set_boolean (win->keyfile, "tenace", "show_played_cards",
		win->show_played_cards);

	/* Hands */
	switch (win->show_hands) {
		case seat_all:
			g_key_file_set_string (win->keyfile, "tenace", "show_hands", "all");
			break;
		case north_south:
			g_key_file_set_string (win->keyfile, "tenace", "show_hands", "ns");
			break;
		case east_west:
			g_key_file_set_string (win->keyfile, "tenace", "show_hands", "ew");
			break;
		default:
			g_key_file_set_string (win->keyfile, "tenace", "show_hands", "none");
	};
	switch (win->show_dd_scores) {
		case seat_all:
			g_key_file_set_string (win->keyfile, "tenace", "show_dd", "all");
			break;
		case north_south:
			g_key_file_set_string (win->keyfile, "tenace", "show_dd", "ns");
			break;
		case east_west:
			g_key_file_set_string (win->keyfile, "tenace", "show_dd", "ew");
			break;
		default:
			g_key_file_set_string (win->keyfile, "tenace", "show_dd", "none");
	};

	struct stat buf;
	if (stat (g_get_user_config_dir (), &buf) == -1)
		mkdir (g_get_user_config_dir (), 0777);

	char rcfile[1024];
	snprintf (rcfile, sizeof (rcfile), "%s/%s",
		g_get_user_config_dir (), "/tenacerc");
	FILE *f;
	if (!(f = fopen(rcfile, "w"))) {
		perror (rcfile);
		return 0;
	}
	char *data = g_key_file_to_data (win->keyfile, NULL, NULL);
	fprintf (f, data);
	fclose (f);
	g_free (data);

	return 1;
}

#endif /* OPTIONS_H */
