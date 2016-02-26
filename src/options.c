/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2012 Christoph Berg <cb@df7cb.de>
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

#include <assert.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "functions.h"
#include "options.h"
#include "window_board.h"
#include "window_card.h"

static char *entry_name[] = { "entry_west", "entry_north", "entry_east", "entry_south" };

static GtkWidget *window_options = NULL;
static GtkListStore *board_store = NULL;

/* set options window contents from program data */
void
window_options_board_populate (void) /* no parameter as it is called from window_board.c */
{
	if (! window_options)
		return;

	board *b = CUR_BOARD;
	GtkWidget *w = get_widget ("entry_title");
	if (b->name)
		gtk_entry_set_text (GTK_ENTRY (w), b->name->str);
	int i;
	for (i = 0; i < 4; i++) {
		w = get_widget (entry_name[i]);
		if (b->hand_name[i])
			gtk_entry_set_text (GTK_ENTRY (w), b->hand_name[i]->str);
	}
}

void
window_options_board_list_populate (void) /* no parameter as it is called from window_board.c */
{
	if (! window_options)
		return;
	assert (board_store);

	PROTECT_BEGIN;
	GtkWidget *w = get_widget ("board_list");
	gtk_tree_view_set_model (GTK_TREE_VIEW (w), GTK_TREE_MODEL (board_store));

	gtk_list_store_clear (board_store);
	GtkTreeIter iter;

	int i;
	GString *name = g_string_new (NULL);
	for (i = 0; i < win->n_boards; i++) {
		board *b = win->boards[i];
		gtk_list_store_append (board_store, &iter);
		g_string_printf (name, "%s (%s)", b->name->str,
			contract_string (b->level, b->trumps, b->declarer, b->doubled));
		gtk_list_store_set (board_store, &iter,
				0, i + 1,
				1, name->str,
				-1);
	}
	g_string_free (name, TRUE);
	PROTECT_END;
}

/* set program data from options window */
static void
apply_options (GtkWidget *window_options)
{
	/* Card display */
	GtkWidget *w = get_widget ("show_played_cards");
	win->show_played_cards = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w));

	w = get_widget ("show_as_cards");
	int style = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (w)) ?
		HAND_DISPLAY_STYLE_CARDS : HAND_DISPLAY_STYLE_TEXT;
	window_card_set_style (style);
	w = get_widget ("svg_file");
	gchar *fname = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (w));
	if (fname && strcmp (win->svg, fname)) { /* svg file changed */
		if (win->svg)
			g_free (win->svg);
		win->svg = fname;
	}
	w = get_widget ("spinbutton_card_width");
	int card_width = gtk_spin_button_get_value_as_int (GTK_SPIN_BUTTON (w));
	board_window_set_style (win, style, card_width);
	board_window_apply_style (win);

	/* Hands */
	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(get_widget ("show_dd_all"))))
	{
		win->show_dd_scores = seat_all;
	} else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(get_widget ("show_dd_ns"))))
	{
		win->show_dd_scores = north_south;
	} else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(get_widget ("show_dd_ew"))))
	{
		win->show_dd_scores = east_west;
	} else {
		win->show_dd_scores = seat_none;
	}

	if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(get_widget ("show_hand_all"))))
	{
		win->show_hands = seat_all;
	} else if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON
		(get_widget ("show_hand_ns"))))
	{
		win->show_hands = north_south;
	} else {
		win->show_hands = east_west;
	}

	/* Current board */
	board *b = CUR_BOARD;
	w = get_widget ("entry_title");
	g_string_printf (b->name, "%s", gtk_entry_get_text (GTK_ENTRY (w)));
	int i;
	for (i = 0; i < 4; i++) {
		w = get_widget (entry_name[i]);
		g_string_printf (b->hand_name[i],
			"%s", gtk_entry_get_text (GTK_ENTRY (w)));
	}

	/* Board list */
	board **new_boards = malloc (win->n_boards_alloc * sizeof (board *));
	//int new_cur = 0;
	i = 0;
	GtkTreeIter iter;
	gtk_tree_model_get_iter_first (GTK_TREE_MODEL (board_store), &iter);
	do {
		GValue val = { 0 };
		gtk_tree_model_get_value (GTK_TREE_MODEL (board_store), &iter, 0, &val);
		int n = g_value_get_int (&val) - 1;
		assert (0 <= n && n < win->n_boards);
		new_boards[i] = win->boards[n];
		/*if (n == win->cur)
			new_cur = i;*/
		i++;
	} while (gtk_tree_model_iter_next (GTK_TREE_MODEL (board_store), &iter));
	free (win->boards);
	win->boards = new_boards;

	show_board(CUR_BOARD, REDRAW_HANDS | REDRAW_NAMES | REDRAW_CONTRACT |
			REDRAW_BOARD_LIST);
}

/* read config from disk */
int
read_config (window_board_t *win)
{
	char rcfile[1024];
	snprintf (rcfile, sizeof (rcfile), "%s/%s",
		g_get_user_config_dir (), "tenacerc");

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
		mkdir (g_get_user_config_dir ()
#ifndef _WIN32
				, 0777
#endif
				);

	char rcfile[1024];
	snprintf (rcfile, sizeof (rcfile), "%s/%s",
		g_get_user_config_dir (), "/tenacerc");
	FILE *f;
	if (!(f = fopen(rcfile, "w"))) {
		perror (rcfile);
		return 0;
	}
	char *data = g_key_file_to_data (win->keyfile, NULL, NULL);
	fputs (data, f);
	fclose (f);
	g_free (data);

	return 1;
}

/* callbacks */

/* create options window and fill it */
G_MODULE_EXPORT void
on_options1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (window_options)
		return;

	window_options = get_widget ("window_options");
	gtk_widget_show (window_options);

	/* Tab 1: Card display */
	GtkWidget *w = get_widget ("show_played_cards");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), win->show_played_cards);

	w = get_widget (win->hand_display_style == HAND_DISPLAY_STYLE_CARDS ?
			"show_as_cards" : "show_as_text");
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);

	w = get_widget ("svg_file");
	if (win->svg)
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (w), win->svg);

	w = get_widget ("spinbutton_card_width");
	GtkAdjustment *adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (w));
	gtk_adjustment_configure (adj, win->card_width,
			10, 10000, 10, 100, 0);

	/* Tab 2: Hands */
	switch (win->show_hands) {
		case seat_none: /* not yet implemented - is this useful? */
			w = get_widget ("show_hand_none");
			break;
		case east_west:
			w = get_widget ("show_hand_ew");
			break;
		case north_south:
			w = get_widget ("show_hand_ns");
			break;
		default:
			w = get_widget ("show_hand_all");
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);

	switch (win->show_dd_scores) {
		case seat_none:
			w = get_widget ("show_dd_none");
			break;
		case east_west:
			w = get_widget ("show_dd_ew");
			break;
		case north_south:
			w = get_widget ("show_dd_ns");
			break;
		default:
			w = get_widget ("show_dd_all");
	}
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (w), TRUE);

	/* Tab 3: Current board */
	window_options_board_populate ();

	/* Tab 4: Board list */
	if (! board_store) {
		board_store = gtk_list_store_new (2,
			G_TYPE_INT, G_TYPE_STRING);
	}

	w = get_widget ("board_list");
	gtk_tree_view_set_model (GTK_TREE_VIEW (w), GTK_TREE_MODEL (board_store));

	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (
			_("#"), renderer, "markup", 0, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (w), column);

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes (
			_("Title"), renderer, "markup", 1, NULL);
	gtk_tree_view_append_column (GTK_TREE_VIEW (w), column);

	/* Tab 5: Generate */
	w = get_widget ("options_generate_number");
	adj = gtk_spin_button_get_adjustment (GTK_SPIN_BUTTON (w));
	gtk_adjustment_configure (adj, 1, 1, 10000, 1, 10, 0);

	window_options_board_list_populate ();
}


G_MODULE_EXPORT gboolean
on_window_options_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	window_options = NULL;
	return FALSE;
}


G_MODULE_EXPORT void
on_options_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide (GTK_WIDGET (window_options));
	window_options = NULL;
}


G_MODULE_EXPORT void
on_options_generate_go_clicked         (GtkButton       *button,
                                        gpointer         user_data)
{
#ifndef _WIN32
	GtkWidget *number = get_widget ("options_generate_number");
	int n = gtk_spin_button_get_value (GTK_SPIN_BUTTON (number));
	char nn[10];
	snprintf (nn, sizeof(nn), "%d", n);
	GtkWidget *view = get_widget ("options_generate_entry");
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (view));
	GtkTextIter start, end;
	gtk_text_buffer_get_iter_at_offset (buffer, &start, 0);
	gtk_text_buffer_get_iter_at_offset (buffer, &end, -1);
	char *text = gtk_text_buffer_get_text (buffer, &start, &end, FALSE);

	int wfd[2], rfd[2];
	pid_t cpid;

	if (pipe(wfd) == -1 || pipe(rfd) == -1) {
		perror("pipe");
		exit(EXIT_FAILURE);
	}

	cpid = fork();
	if (cpid == -1) {
		perror("fork");
		exit(EXIT_FAILURE);
	}

	if (cpid == 0) {    /* Child reads from pipe */
		close (wfd[1]);          /* Close unused write end */
		close (rfd[0]);
		dup2 (0, wfd[0]);
		dup2 (1, rfd[1]);
		execlp ("dealer", "dealer", "-v", "-p", nn, NULL);
		perror ("execlp");
		_exit(EXIT_FAILURE);

	} else {
		close (wfd[0]);          /* Close unused read end */
		close (rfd[1]);
		if (write (wfd[1], text, strlen (text)) == -1) {
			perror ("write");
			return;
		}
		close (wfd[1]);          /* Reader will see EOF */
	}

	int i;
	do {
		char buf[100];
		i = read (rfd[0], buf, sizeof (buf));
		buf[i == sizeof (buf) ? sizeof (buf) - 1 : i] = '\0';
		puts (buf);
	} while (i > 0);
	close (rfd[0]);

	g_free (text);
#endif
}


G_MODULE_EXPORT void
on_options_apply_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	apply_options (window_options);
}


G_MODULE_EXPORT void
on_options_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	on_options_apply_clicked (button, user_data);
	gtk_widget_hide (GTK_WIDGET (window_options));
	window_options = NULL;
}


