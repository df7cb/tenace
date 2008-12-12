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

#include <assert.h>
#include <cairo/cairo-pdf.h>
#include <math.h>

#include "functions.h"
#include "window_board.h"

static GtkPrintSettings *settings = NULL;

/* standard printing */

#define STRIDE 0.25 * 72
#define L 1.0 * 72
#define C 2.0 * 72
#define R 3.0 * 72
#define T 1.0 * 72
#define M 2.0 * 72
#define B 3.0 * 72

static void
draw_page (GtkPrintOperation *operation,
           GtkPrintContext   *context,
           gint               page_nr,
           gpointer           user_data)

{
	cairo_t *cr;
	cr = gtk_print_context_get_cairo_context (context);

	board *b = CUR_BOARD;

	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size (cr, 12);
	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);

	cairo_move_to (cr, 0.1 * 72, 0.2 * 72);
	cairo_show_text (cr, b->name->str);
	cairo_stroke (cr);

	cairo_move_to (cr, 0.1 * 72, 0.5 * 72);

	cairo_show_text (cr, contract_string(b->level, b->trumps, b->declarer, b->doubled));
	cairo_stroke (cr);

	cairo_move_to (cr, C, T+1*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, north, spade));
	cairo_move_to (cr, C, T+2*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, north, heart));
	cairo_move_to (cr, C, T+3*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, north, diamond));
	cairo_move_to (cr, C, T+4*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, north, club));

	cairo_move_to (cr, L, M+1*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, west, spade));
	cairo_move_to (cr, L, M+2*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, west, heart));
	cairo_move_to (cr, L, M+3*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, west, diamond));
	cairo_move_to (cr, L, M+4*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, west, club));

	cairo_move_to (cr, R, M+1*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, east, spade));
	cairo_move_to (cr, R, M+2*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, east, heart));
	cairo_move_to (cr, R, M+3*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, east, diamond));
	cairo_move_to (cr, R, M+4*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, east, club));

	cairo_move_to (cr, C, B+1*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, south, spade));
	cairo_move_to (cr, C, B+2*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, south, heart));
	cairo_move_to (cr, C, B+3*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, south, diamond));
	cairo_move_to (cr, C, B+4*STRIDE);
	cairo_show_text (cr, hand_suit_string (b, south, club));
	cairo_stroke (cr);

	cairo_move_to (cr, C, M);
	cairo_line_to (cr, C, R);
	cairo_line_to (cr, B, R);
	cairo_line_to (cr, B, C);
	cairo_close_path (cr);
	cairo_stroke (cr);
}

static void
begin_print (GtkPrintOperation *operation,
	     GtkPrintContext   *context,
	     gpointer           user_data)
{
	gtk_print_operation_set_n_pages (operation, 1);
}

void
on_menu_print_activate (void)
{
	GtkPrintOperation *print;
	GtkPrintOperationResult res;

	print = gtk_print_operation_new ();

	if (settings != NULL)
		gtk_print_operation_set_print_settings (print, settings);

	g_signal_connect (print, "begin_print", G_CALLBACK (begin_print), NULL);
	g_signal_connect (print, "draw_page", G_CALLBACK (draw_page), NULL);

	res = gtk_print_operation_run (print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
			GTK_WINDOW (win->window), NULL);

	if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
	{
		if (settings != NULL)
			g_object_unref (settings);
		settings = g_object_ref (gtk_print_operation_get_print_settings (print));
	}

	g_object_unref (print);
}

/* magic cards */

static void
arrow (cairo_t *cr, int n, int background, seat dir, double size)
{
#define ARHEAD 0.0
#define ARWIDTH 0.2
	cairo_save (cr);
	cairo_translate (cr, 0.5*size, 0.5*size);

	/* background box */
	if (background) {
		cairo_set_source_rgb (cr, 0.8, 0.8, 0.8);
		cairo_move_to (cr, -0.5*size, -0.5*size);
		cairo_line_to (cr, 0.5*size, -0.5*size);
		cairo_line_to (cr, 0.5*size, 0.5*size);
		cairo_line_to (cr, -0.5*size, 0.5*size);
		cairo_close_path (cr);
		cairo_fill (cr);
	}

	/* arrow */
	cairo_save (cr);
	cairo_rotate (cr, ((int)dir - 2) * M_PI_2);

	if (n % 2)
		cairo_set_source_rgb (cr, 0.7, 0.4, 0.4); /* red */
	else
		cairo_set_source_rgb (cr, 0.4, 0.4, 0.7); /* blue */

	cairo_move_to (cr, 0.0, -0.5*size);
	cairo_line_to (cr, 0.5*size, -ARHEAD*size);
	cairo_line_to (cr, ARWIDTH*size, -ARHEAD*size);
	cairo_line_to (cr, ARWIDTH*size, 0.5*size);
	cairo_line_to (cr, -ARWIDTH*size, 0.5*size);
	cairo_line_to (cr, -ARWIDTH*size, -ARHEAD*size);
	cairo_line_to (cr, -0.5*size, -ARHEAD*size);
	cairo_close_path (cr);
	cairo_fill (cr);

	cairo_restore (cr);

	/* board number */
	char buf[8];
	cairo_text_extents_t ext;
	snprintf (buf, sizeof (buf), "%d", n);
	cairo_select_font_face (cr, "Sans",
		CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, size / 2.0);
	cairo_text_extents (cr, buf, &ext);

	cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
	cairo_move_to (cr, -ext.width / 2 - ext.x_bearing, ext.height/2);
	cairo_show_text (cr, buf);

	cairo_restore (cr);
}

static void
cairo_clip_rectangle (cairo_t *cr, double x0, double y0, double x1, double y1)
{
	cairo_move_to (cr, x0, y0);
	cairo_line_to (cr, x1, y0);
	cairo_line_to (cr, x1, y1);
	cairo_line_to (cr, x0, y1);
	cairo_close_path (cr);
	cairo_clip (cr);
}

/*  row_height
 ***************** border
 *xxx   xxx   xxx* arrow_size
 *xxx   xxx   xxx*
 *   xxx   xxx   *
 *   xxx   xxx   * col_width
 * arrow_columns *
 *    TEXTSEP    *
 *               *
 *               *
 *               *
 *               *
 *****************/

#define TEXTSEP 20.0

struct magic_config_t {
	/* custom print settings */
	GtkSpinButton* magic_columns, * magic_rows;
	GtkToggleButton* magic_no_header, * magic_page_header, * magic_suit_header;
	GtkToggleButton* magic_mark_cards;
	GtkSpinButton* magic_from, * magic_to;
	GtkSpinButton* magic_border;
	int columns, rows;
	int header;
	int mark_cards;
	int from, to;
	double border;
	/* metrics */
	double col_width, row_height;
	int arrow_columns;
	double arrow_size;
};
static struct magic_config_t mc;

static void
magic_card_half (cairo_t *cr, window_board_t *win, card c)
{
	int n_boards = mc.to - mc.from + 1;
	int n;
	for (n = 0; n < n_boards; n++) {
		cairo_save (cr);
		int col = n % mc.arrow_columns;
		int row = n / mc.arrow_columns;
		cairo_translate (cr, col * mc.arrow_size + mc.border,
				row * mc.arrow_size + mc.border);

		board *b = win->boards[n + mc.from - 1];
		arrow (cr, n + mc.from, (col + row) % 2, b->dealt_cards[c], mc.arrow_size);
		cairo_restore (cr);

		/* draw title */
		cairo_text_extents_t ext;
		cairo_save (cr);

		cairo_select_font_face (cr, "Sans",
			CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size (cr, 8.0);
		cairo_clip_rectangle (cr, mc.border, mc.border,
				mc.row_height - mc.border, mc.col_width - mc.border);
		cairo_move_to (cr, mc.border, 0.5 * mc.col_width - 2.0);
		cairo_show_text (cr, win->title);
		cairo_text_extents (cr, win->title, &ext);

		/* clip subtitle to card width - title width */
		cairo_clip_rectangle (cr, mc.border, mc.border,
			mc.row_height - ext.x_advance - mc.border - 4.0, mc.col_width - mc.border);
		cairo_move_to (cr, mc.border, 0.5 * mc.col_width + 8.0);
		cairo_show_text (cr, win->subtitle);
		cairo_stroke (cr);

		cairo_restore (cr);
	}
}

static void
magic_card (cairo_t *cr, window_board_t *win, card c)
{
	/* upper part */
	magic_card_half (cr, win, c);
	/* lower part */
	cairo_save (cr);
	cairo_translate (cr, mc.row_height, mc.col_width);
	cairo_rotate (cr, M_PI);
	magic_card_half (cr, win, c);
	cairo_restore (cr);
}

static void
magic_draw_page (GtkPrintOperation *operation,
           GtkPrintContext   *context,
           gint               page_nr,
           gpointer           user_data)

{
	cairo_t *cr;
	cr = gtk_print_context_get_cairo_context (context);

	static int cc, print_head;
	if (page_nr == 0) { /* restart printing */
		cc = 51;
		print_head = (mc.header > 0);
	}
	if (mc.header == 1) /* new page */
		print_head = 1;

	int r, c;
	for (c = 0; c < mc.columns; c++) {
		for (r = 0; r < mc.rows && cc >= 0; r++) {
			if (print_head && mc.header == 1) { /* page heading */
				cairo_set_font_size (cr, 10.0);

				int r1, c1;
				int cc1 = cc;
				for (c1 = 0; c1 < mc.columns; c1++) {
					for (r1 = 0; r1 < mc.rows; r1++) {
						if (c1 == 0 && r1 == 0 || cc1 < 0)
							continue;
						cairo_move_to (cr, c1 * (mc.col_width - 2.0*mc.border) / mc.columns + mc.border,
								r1 * (mc.row_height - 2.0*mc.border) / mc.rows + mc.border + 10.0);
						cairo_select_font_face (cr, "Symbol",
							CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
						cairo_show_text (cr, trump_str[SUIT(cc1)]);
						cairo_select_font_face (cr, "Sans",
							CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
						cairo_show_text (cr, rank_string (RANK(cc1)));
						cc1--;
					}
				}
				print_head = 0;
				continue;
			}
			if (print_head && mc.header == 2) { /* suit heading */
				cairo_move_to (cr, (c + 0.5) * mc.col_width - 15.0,
						(r + 0.5) * mc.row_height + 30.0);
				cairo_select_font_face (cr, "Symbol",
					CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
				cairo_set_font_size (cr, 40.0);
				cairo_show_text (cr, trump_str[SUIT(cc)]);
				print_head = 0;
				continue;
			}

			cairo_save (cr);
			cairo_translate (cr, mc.col_width * c, mc.row_height * (r + 1));
			cairo_rotate (cr, -M_PI_2);
			magic_card (cr, win, cc);

			if (mc.mark_cards) {
				cairo_move_to (cr, 6, -2);
				cairo_set_font_size (cr, 10);
				cairo_select_font_face (cr, "Symbol",
					CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
				cairo_show_text (cr, trump_str[SUIT(cc)]);
				cairo_select_font_face (cr, "Sans",
					CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);
				cairo_show_text (cr, _(rank_string (RANK(cc))));
			}

			cairo_restore (cr);
			
			cc--; /* next card */
			if (RANK(cc) == cardA && mc.header == 2)
				print_head = 1;
		}
	}
}

static GObject *
magic_custom_create (GtkPrintOperation *operation,
	     gpointer           user_data)
{
	GladeXML *xml = glade_xml_new(PACKAGE_DATA_DIR "/" PACKAGE "/" PACKAGE ".glade", "magic_options_table", NULL);
	GtkWidget *table = glade_xml_get_widget (xml, "magic_options_table");
	assert (table);
	mc.magic_rows = GTK_SPIN_BUTTON (glade_xml_get_widget (xml, "magic_rows"));
	mc.magic_columns = GTK_SPIN_BUTTON (glade_xml_get_widget (xml, "magic_columns"));
	mc.magic_no_header = GTK_TOGGLE_BUTTON (glade_xml_get_widget (xml, "magic_no_header"));
	mc.magic_page_header = GTK_TOGGLE_BUTTON (glade_xml_get_widget (xml, "magic_page_header"));
	mc.magic_suit_header = GTK_TOGGLE_BUTTON (glade_xml_get_widget (xml, "magic_suit_header"));
	mc.magic_mark_cards = GTK_TOGGLE_BUTTON (glade_xml_get_widget (xml, "magic_mark_cards"));
	mc.magic_from = GTK_SPIN_BUTTON (glade_xml_get_widget (xml, "magic_from"));
	gtk_spin_button_set_range (mc.magic_from, 1, win->n_boards);
	mc.magic_to = GTK_SPIN_BUTTON (glade_xml_get_widget (xml, "magic_to"));
	gtk_spin_button_set_range (mc.magic_to, 1, win->n_boards);
	gtk_spin_button_set_value (mc.magic_to, win->n_boards);
	mc.magic_border = GTK_SPIN_BUTTON (glade_xml_get_widget (xml, "magic_border"));
	g_object_unref (xml);
	return (GObject *) table;
}

static void
magic_custom_apply (GtkPrintOperation *operation, GtkWidget *table, gpointer user_data)
{
	mc.columns = gtk_spin_button_get_value_as_int (mc.magic_columns);
	mc.rows = gtk_spin_button_get_value_as_int (mc.magic_rows);
	mc.header = gtk_toggle_button_get_active (mc.magic_no_header) ? 0 :
			      gtk_toggle_button_get_active (mc.magic_page_header) ? 1 : 2;
	mc.mark_cards = gtk_toggle_button_get_active (mc.magic_mark_cards);
	mc.from = gtk_spin_button_get_value_as_int (mc.magic_from);
	mc.to = gtk_spin_button_get_value_as_int (mc.magic_to);
	if (mc.to < mc.from)
		mc.to = mc.from;
	mc.border = gtk_spin_button_get_value_as_float (mc.magic_border);
}

static void
magic_begin_print (GtkPrintOperation *operation,
	     GtkPrintContext   *context,
	     gpointer           user_data)
{
	if (mc.header == 0) {
		gtk_print_operation_set_n_pages
			(operation, ceil (52.0 / (mc.columns * mc.rows)));
	} else if (mc.header == 1) {
		gtk_print_operation_set_n_pages
			(operation, ceil (52.0 / (mc.columns * mc.rows -
						  (mc.columns == 1 && mc.columns == 1 ? 0 : 1))));
	} else {
		gtk_print_operation_set_n_pages
			(operation, ceil (56.0 / (mc.columns * mc.rows)));
	}
	gtk_print_operation_set_use_full_page (operation, TRUE);
	gtk_print_operation_set_show_progress (operation, TRUE);
	//gtk_print_operation_set_export_filename (operation, "foo");
	mc.col_width = gtk_print_context_get_width (context) / mc.columns;
	mc.row_height = gtk_print_context_get_height (context) / mc.rows;

	/* find fitting number of arrow columns and corresponding arrow size */
	int n_boards = mc.to - mc.from + 1;
	mc.arrow_columns = 0;
	do {
		mc.arrow_columns++;
		mc.arrow_size = (mc.row_height - 2*mc.border) / mc.arrow_columns;
		int arrow_rows = ceil ((double) n_boards / mc.arrow_columns);
		if (mc.arrow_size * arrow_rows <= mc.col_width/2.0 - mc.border - TEXTSEP/2.0)
			break;
	} while (mc.arrow_columns <= n_boards);
}

void
on_menu_file_magic_activate (void)
{
	GtkPrintOperation *print;
	GtkPrintOperationResult res;

	print = gtk_print_operation_new ();

	if (settings != NULL) 
		gtk_print_operation_set_print_settings (print, settings);

	gtk_print_operation_set_custom_tab_label (print, _("Labels"));

	g_signal_connect (print, "begin_print", G_CALLBACK (magic_begin_print), NULL);
	g_signal_connect (print, "draw_page", G_CALLBACK (magic_draw_page), NULL);
	g_signal_connect (print, "create-custom-widget", G_CALLBACK (magic_custom_create), NULL);
	g_signal_connect (print, "custom-widget-apply", G_CALLBACK (magic_custom_apply), NULL);

	res = gtk_print_operation_run (print, GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,
			GTK_WINDOW (win->window), NULL);

	if (res == GTK_PRINT_OPERATION_RESULT_APPLY)
	{
		if (settings != NULL)
			g_object_unref (settings);
		settings = g_object_ref (gtk_print_operation_get_print_settings (print));
	}

	g_object_unref (print);
}

