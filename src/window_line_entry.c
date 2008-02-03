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

#include <assert.h>

#include "bridge.h"
#include "file.h"
#include "interface.h"
#include "support.h"
#include "window_board.h"
#include "window_card.h"

static GtkWidget *window_line_entry = NULL;
static GtkEntry *line_entry = NULL;

void board_set_from_line_entry(board *b)
{
	assert (window_line_entry);
	const char *line = gtk_entry_get_text(line_entry);
	board_clear(b);
	if (!board_parse_line(line, b, ' ', '.')) {
		board_statusbar(_("Parse error"));
	}
	card_window_update(b->dealt_cards);
	show_board(b, REDRAW_FULL);
}

void line_entry_set_from_board(board *b)
{
	if (!window_line_entry)
		return;

	GString *line = board_format_line(b, ' ', '.');
	gtk_entry_set_text(line_entry, line->str);
	g_string_free(line, TRUE);
}

void window_line_entry_init(board *b)
{
	if (!window_line_entry) {
		window_line_entry = create_window_line_entry ();
		line_entry = GTK_ENTRY(lookup_widget(window_line_entry, "line_entry"));
		gtk_widget_show (window_line_entry);
	}
	if (b)
		line_entry_set_from_board(b);
}

void window_line_entry_delete (board *b)
{
	if (window_line_entry) {
		gtk_widget_destroy (window_line_entry);
		window_line_entry = NULL;
		line_entry = NULL;
	}
}

