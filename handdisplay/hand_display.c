/*
 *  hand display - bridge hand widget for GTK+
 *  Copyright (C) 2007-2013 Christoph Berg <cb@df7cb.de>
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
#include <glib.h>
#include <gtk/gtk.h>
#include <math.h>
#include <string.h>

#include "hand_display.h"
#include "int_int_marshal.h"

/* static data */

static int render_init = 0, card_width = 100, card_height = 0;
static gchar *svg_filename = NULL;
static GdkPixbuf *card_pixbuf[52];
static cairo_surface_t *grey_surface = NULL;
static GtkWidget *drag_win = NULL; /* current drag icon */
static GtkTargetEntry target_entry[1] = { { "card", 0, 0 } };
static GtkTargetList *target_list = NULL;

static char *suit_str[] = {"♣", "♦", "♥", "♠"};
static char *rank_str[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
static double suit_color[4][3] = {
	{ HAND_DISPLAY_CLUBS_FONT    },
	{ HAND_DISPLAY_DIAMONDS_FONT },
	{ HAND_DISPLAY_HEARTS_FONT   },
	{ HAND_DISPLAY_SPADES_FONT   },
};

/* internal functions */

static int
which_card (HandDisplay *handdisp, int x, int y)
{
	/* find rightmost (= topmost) card at cursor position */
	int c;
	int max_x = -1;
	int max = -1;
	for (c = 0; c < (handdisp->mode == HAND_DISPLAY_MODE_HAND_X ? 56 : 52); c++) {
		if ((c >= 52 /* HAND_X */ || handdisp->cards[c] & ~HAND_DISPLAY_INVISIBLE_CARD /* card visible */) &&
				x >= handdisp->l[c] && x <= handdisp->r[c] &&
				y >= handdisp->t[c] && y <= handdisp->b[c]) {
			if (handdisp->r[c] > max_x) {
				max = c;
				max_x = handdisp->r[c];
			}
		}
	}
	return max;
}

static char *
overtricks (int i)
{
	static char buf[4];
	if (i == 0)
		snprintf(buf, 3, "=");
	else
		snprintf(buf, 3, "%+d", i);
	return buf;
}

static void
render_card_init (char *card_fname)
{
	int i;
	if (render_init) {
		for (i = 0; i < 52; i++)
			g_object_unref (card_pixbuf[i]);
		cairo_surface_destroy (grey_surface);
		render_init = 0;
	}

	/* gdk_pixbuf_new_from_file doesn't seem to support .svgz (while
	 * librsvg does), so decompress it here. Code from aisleriot
	 * src/lib/ar-svg.c */
	GFile *cf = g_file_new_for_path (card_fname);
	GFileInfo *info;
	GError *error = NULL;
	if (!(info = g_file_query_info (cf,
					G_FILE_ATTRIBUTE_STANDARD_FAST_CONTENT_TYPE,
					G_FILE_QUERY_INFO_NONE,
					NULL,
					&error))) {
		printf ("%s: %s\n", card_fname, error->message);
		g_object_unref (cf);
		g_error_free (error);
		return;
	}

	const char *type = g_file_info_get_attribute_string (info, G_FILE_ATTRIBUTE_STANDARD_FAST_CONTENT_TYPE);
	char *gz_type = g_content_type_from_mime_type ("application/x-gzip");
	gboolean is_gzip = (type != NULL && g_content_type_is_a (type, gz_type));
	g_free (gz_type);
	g_object_unref (info);

	GInputStream *stream;
	if (!(stream = G_INPUT_STREAM (g_file_read (cf, NULL, &error)))) {
		printf ("%s: %s\n", card_fname, error->message);
		g_object_unref (cf);
		g_error_free (error);
		return;
	}
	g_object_unref (cf);

	if (is_gzip) {
		GZlibDecompressor *decompressor;
		GInputStream *converter_stream;

		decompressor = g_zlib_decompressor_new (G_ZLIB_COMPRESSOR_FORMAT_GZIP);
		converter_stream = g_converter_input_stream_new (stream,
				G_CONVERTER (decompressor));
		g_object_unref (stream);
		stream = converter_stream;
	}

	/* file contains cards in 13 columns (A/2..10/J/Q/K) and 5 rows (C/D/H/S/Jokers) */
	/* actual card height is computed from resulting actual size */
	GdkPixbuf *pb = gdk_pixbuf_new_from_stream_at_scale (stream,
			card_width * 13, -1, TRUE, NULL, &error);
	g_object_unref (stream);
	if (!pb) {
		printf ("%s: %s.\n", card_fname, error->message);
		g_error_free (error);
		return;
	}
	int buf_width = gdk_pixbuf_get_width (pb);
	int buf_height = gdk_pixbuf_get_height (pb);
	card_width = ceil (gdk_pixbuf_get_width (pb) / 13.0);
	card_height = ceil (gdk_pixbuf_get_height (pb) / 5.0);
	for (i = 0; i < 52; i++) {
		card_pixbuf[i] = gdk_pixbuf_new (GDK_COLORSPACE_RGB, TRUE, 8, card_width, card_height);
		if (!card_pixbuf[i]) {
			printf ("%s: rendering card_pixbuf failed\n", card_fname);
			return;
		}
		int col = (i + 1) % 13;
		int row = i / 13;
		gdk_pixbuf_copy_area (pb, buf_width * col / 13.0, buf_height * row / 5.0,
		//gdk_pixbuf_copy_area (pb, card_width * col, card_height * row,
			card_width, card_height, card_pixbuf[i], 0, 0);
	}
	g_object_unref (pb);

	/* construct a alpha channel in card shape for greying out cards */
	grey_surface = cairo_image_surface_create (CAIRO_FORMAT_A8, card_width, card_height);
	cairo_t *ct = cairo_create (grey_surface);
	gdk_cairo_set_source_pixbuf (ct, card_pixbuf[0], 0, 0);
	cairo_paint_with_alpha (ct, 0.3);
	cairo_destroy (ct);

	render_init = 1;
}

static void
render_card (cairo_t *cr, int x, int y, int c, int color)
{
	if (!render_init)
		return;
	assert (0 <= c && c < 52);

	gdk_cairo_set_source_pixbuf (cr, card_pixbuf[c], x, y);
	cairo_paint (cr);

	if (color == HAND_DISPLAY_OLD_CARD) { /* grey out old card */
		cairo_set_source_rgb (cr, HAND_DISPLAY_GREY_FONT);
		cairo_mask_surface (cr, grey_surface, x, y);
	}
	return;
}

static void
draw (GtkWidget *hand, cairo_t *cr)
{
	int x, y;
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	cairo_text_extents_t extents;
	cairo_font_extents_t fextents;
	GtkAllocation allocation;
	gtk_widget_get_allocation (hand, &allocation);

	cairo_select_font_face (cr, "sans-serif", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_BOLD);

	/* "card" mode for drap&drop icon */
	if (handdisp->mode == HAND_DISPLAY_MODE_CARD) {
		if (handdisp->style == HAND_DISPLAY_STYLE_CARDS) {
			if (card_width != handdisp->want_width) { /* adjust window */
				handdisp->want_width = card_width;
				gdk_window_resize (gtk_widget_get_parent_window (hand),
						card_width, card_height);

				/* shaped drag icon
				 * credits to Mirco "MacSlow" Mueller <macslow@bangang.de>
				 * http://macslow.thepimp.net/?p=26 */
				GdkBitmap *pShapeBitmap = (GdkBitmap*) gdk_pixmap_new (NULL, card_width, card_height, 1);
				assert (pShapeBitmap);
				cairo_t *pCairoContext = gdk_cairo_create (pShapeBitmap);
				assert (cairo_status (pCairoContext) == CAIRO_STATUS_SUCCESS);
				render_card (pCairoContext, 0, 0, handdisp->table_card[0], HAND_DISPLAY_CARD);
				cairo_destroy (pCairoContext);
				gdk_window_shape_combine_mask (gtk_widget_get_parent_window (hand), pShapeBitmap, 0, 0);
				g_object_unref ((gpointer) pShapeBitmap);
			}
			render_card (cr, 0, 0, handdisp->table_card[0], HAND_DISPLAY_CARD);
			return;
		}

		char cs[6];
		int c = handdisp->table_card[0];
		int suit = c / 13;
		int rank = c % 13;

		snprintf (cs, 6, "%s%s", suit_str[suit], rank_str[rank]);
		cairo_set_font_size (cr, 20);
		cairo_text_extents (cr, cs, &extents);
#define XPAD 4
#define YPAD 2
		int w = extents.width + 2 * XPAD + 5;
		int h = extents.height + 2 * YPAD;

		cairo_set_source_rgb (cr, HAND_DISPLAY_FOCUS_BG);
		cairo_rectangle (cr, 1, 1, w, h);
		cairo_fill (cr);
		cairo_set_source_rgb (cr, HAND_DISPLAY_FONT);
		cairo_rectangle (cr, 1, 1, w, h);
		cairo_stroke (cr);

		cairo_move_to (cr, XPAD - extents.x_bearing + 1, YPAD - extents.y_bearing + 2);
		cairo_set_source_rgb (cr, suit_color[suit][0],
			suit_color[suit][1], suit_color[suit][2]);
		cairo_show_text (cr, suit_str[suit]);
		cairo_set_source_rgb (cr, HAND_DISPLAY_FONT);
		cairo_show_text (cr, rank_str[rank]);

		if (w + 2 != handdisp->want_width) { /* adjust window */
			handdisp->want_width = w + 2;
			gdk_window_resize (gtk_widget_get_parent_window (hand), w + 2, h + 2);
		}

		return;
	}

	cairo_set_source_rgb (cr, HAND_DISPLAY_TABLE_BG);
	cairo_rectangle (cr, 0, 0, allocation.width, allocation.height);
	cairo_fill (cr);

	/* "table" mode for displaying the already played cards in the middle of the screen */

	if (handdisp->mode == HAND_DISPLAY_MODE_TABLE &&
	    handdisp->style == HAND_DISPLAY_STYLE_CARDS) {
		int i;
		for (i = 0; i < 4; i++) {
			switch (handdisp->table_seat[i]) {
				case 1: x = allocation.width / 2 - card_width + 5;
				/*W*/	y = (allocation.height - card_height) / 2 + 5;
					break;
				case 2: x = (allocation.width - card_width) / 2 - 2;
				/*N*/	y = MAX (allocation.height / 2 - card_height + 10, 0);
					break;
				case 3: x = allocation.width / 2 - 5;
				/*E*/	y = (allocation.height - card_height) / 2 - 5;
					break;
				case 4: x = (allocation.width - card_width) / 2 + 2;
				/*S*/	y = MIN (allocation.height / 2 - 10, allocation.height - card_height);
					break;
				default:
					return; /* stop here */
			}

			render_card (cr, x, y, handdisp->table_card[i], HAND_DISPLAY_CARD);

		}
		return;
	}

	if (handdisp->mode == HAND_DISPLAY_MODE_TABLE) { /* text */
		cairo_set_font_size (cr, 20);

		int i;
		for (i = 0; i < 4; i++) {
			char cs[6];
			if (!handdisp->table_seat[i])
				return;
			int c = handdisp->table_card[i];
			int suit = c / 13;
			int rank = c % 13;
			snprintf (cs, 6, "%s%s", suit_str[suit], rank_str[rank]);
			cairo_text_extents (cr, cs, &extents);

#define XOFF 38
#define YOFF 23
#define WIDTH 52
#define HEIGHT 28
#define RAD 5
			switch (handdisp->table_seat[i]) { /* middle point */
				case 1: x = allocation.width / 2 - XOFF;
				/*W*/	y = allocation.height / 2;
					break;
				case 2: x = allocation.width / 2;
				/*N*/	y = allocation.height / 2 - YOFF;
					break;
				case 3: x = allocation.width / 2 + XOFF;
				/*E*/	y = allocation.height / 2;
					break;
				case 4: x = allocation.width / 2;
				/*S*/	y = allocation.height / 2 + YOFF;
					break;
				default:
					return; /* stop here */
			}

			cairo_new_path (cr);
			cairo_arc_negative (cr, x-WIDTH/2+RAD, y+HEIGHT/2 - RAD, RAD, M_PI, M_PI_2);
			cairo_arc_negative (cr, x+WIDTH/2-RAD, y+HEIGHT/2 - RAD, RAD, M_PI_2, 0.0);
			cairo_arc_negative (cr, x+WIDTH/2-RAD, y-HEIGHT/2 + RAD, RAD, 0.0, -M_PI_2);
			cairo_arc_negative (cr, x-WIDTH/2+RAD, y-HEIGHT/2 + RAD, RAD, -M_PI_2, M_PI);
			cairo_close_path (cr);
			cairo_set_source_rgb (cr, HAND_DISPLAY_FOCUS_BG);
			cairo_fill_preserve (cr);
			cairo_set_source_rgb (cr, HAND_DISPLAY_FONT);
			cairo_stroke (cr);

			cairo_move_to (cr, x - extents.width / 2 - 3,
					y + extents.height / 2 - 3);
			cairo_set_source_rgb (cr, suit_color[suit][0],
				suit_color[suit][1], suit_color[suit][2]);
			cairo_show_text (cr, suit_str[suit]);
			cairo_set_source_rgb (cr, HAND_DISPLAY_FONT);
			cairo_show_text (cr, rank_str[rank]);
		}
		return;
	}

	/* normal hands display */

	/* compute cached best card score for this hand */
	if (handdisp->best_card_score == HAND_DISPLAY_NO_SCORE) {
		int c;
		handdisp->best_card_score = handdisp->card_score_neg ? 14 : -14;
		for (c = 0; c < 52; c++) {
			if (handdisp->card_score_neg ?
				(handdisp->card_score[c] < handdisp->best_card_score) :
				(handdisp->card_score[c] > handdisp->best_card_score))
					handdisp->best_card_score = handdisp->card_score[c];
		}
	}

	/* "cards" style */

	if (handdisp->style == HAND_DISPLAY_STYLE_CARDS && handdisp->mode != HAND_DISPLAY_MODE_HAND_X) {
		/* we do not support MODE_X here, yet we still allow setting
		 * STYLE_CARDS there to have the right drag icon */
		y = MAX (allocation.height - card_height - 5, 15);
		int n = 0;
		int suit;
		for (suit = 0; suit < 4; suit++) {
			int c;
			for (c = 13 * (handdisp->suits[suit] + 1) - 1; c >= 13 * handdisp->suits[suit]; c--) {
				if (handdisp->cards[c]) {
					x = floor (n++ * (allocation.width - card_width) / 12.0);

					int sc = handdisp->card_score[c];
					int yy = c == handdisp->cur_focus ? y - 15 :
						(sc != HAND_DISPLAY_NO_SCORE &&
						 handdisp->best_card_score == sc ? y - 5 : y);
					yy = MAX (yy, 0);
					handdisp->l[c] = x;
					handdisp->r[c] = x + card_width;
					handdisp->t[c] = y - 15;
					handdisp->b[c] = y + card_height;

					if (handdisp->cards[c] & HAND_DISPLAY_INVISIBLE_CARD)
						continue;

					render_card (cr, x, yy, c, handdisp->cards[c]);

					if (handdisp->cards[c] == HAND_DISPLAY_HILIGHT_CARD) { /* draw triangular arrow */
						cairo_set_source_rgb (cr, HAND_DISPLAY_HILIGHT_FONT);
						cairo_move_to (cr, x + 3 + (allocation.width - card_width - 10) / 24.0, yy);
						cairo_rel_line_to (cr, -5, -5);
						cairo_rel_line_to (cr, 10, 0);
						cairo_fill (cr);
					}

					/* show card score */
					if (handdisp->card_score[c] == HAND_DISPLAY_NO_SCORE)
						continue;
					char *buf = overtricks (handdisp->card_score[c]);
					static int text_h = 0;
					if (!text_h) {
						cairo_font_extents (cr, &fextents);
						text_h = fextents.ascent;
					}
					cairo_text_extents (cr, buf, &extents);
					cairo_move_to (cr, x + 1 + text_h,
							yy + 30 + extents.width);
					cairo_set_source_rgb (cr, HAND_DISPLAY_DD_FONT);
					cairo_save (cr);
					cairo_rotate (cr, -G_PI_2);
					cairo_show_text (cr, buf);
					cairo_restore (cr);
				}
			}
		}

		return;
	}

	/* "text" style */

	/* draw suit symbols */
	cairo_set_font_size (cr, 20);
	int suit_width = 0;
	int suit;
	for (suit = 0; suit < 4; suit++) {
		x = 0;
		y = floor ((double) allocation.height * (3.8 - suit) / 4.0);
		cairo_move_to (cr, x, y);
		cairo_text_extents (cr, suit_str[suit], &extents);
		if (extents.x_advance > suit_width)
			suit_width = extents.x_advance;
		cairo_set_source_rgb (cr, suit_color[suit][0], suit_color[suit][1], suit_color[suit][2]);
		cairo_show_text (cr, suit_str[suit]);
	}

	/* draw cards */
	for (suit = 0; suit < 4; suit++) {
		x = 4 + suit_width;
		y = floor ((double) allocation.height * (3.8 - suit) / 4.0);
		cairo_move_to (cr, x, y);

		int c;
		for (c = 13 * suit + 12; c >= 13 * suit; c--) {
			if (handdisp->cards[c]) {
				cairo_text_extents (cr, rank_str[c % 13], &extents);
				handdisp->l[c] = x + extents.x_bearing;
				handdisp->r[c] = x + extents.x_bearing + extents.width;
				handdisp->t[c] = y + extents.y_bearing;
				handdisp->b[c] = y + extents.y_bearing + extents.height;
				if (!(handdisp->cards[c] & HAND_DISPLAY_INVISIBLE_CARD)) {
					if (c == handdisp->cur_focus) {
						cairo_set_source_rgb (cr, HAND_DISPLAY_FOCUS_BG);
						cairo_rectangle (cr, handdisp->l[c] - 1, handdisp->t[c] - 1,
							extents.width + 2, extents.height + 2);
						cairo_fill (cr);
					}
					if (handdisp->card_score[c] == HAND_DISPLAY_NO_SCORE) {
						if (handdisp->cards[c] == HAND_DISPLAY_CARD)
							cairo_set_source_rgb (cr, HAND_DISPLAY_FONT);
						else if (handdisp->cards[c] == HAND_DISPLAY_GREY_CARD)
							cairo_set_source_rgb (cr, HAND_DISPLAY_GREY_FONT);
						else if (handdisp->cards[c] == HAND_DISPLAY_OLD_CARD)
							cairo_set_source_rgb (cr, HAND_DISPLAY_OLD_FONT);
						else if (handdisp->cards[c] == HAND_DISPLAY_HILIGHT_CARD)
							cairo_set_source_rgb (cr, HAND_DISPLAY_HILIGHT_FONT);
					} else {
						/* invert colors if the score is for the opps */
						if (handdisp->card_score_neg ^ (handdisp->card_score[c] >= 0))
							if (handdisp->best_card_score == handdisp->card_score[c])
								cairo_set_source_rgb (cr, HAND_DISPLAY_BEST_POS_FONT);
							else
								cairo_set_source_rgb (cr, HAND_DISPLAY_POS_FONT);
						else
							if (handdisp->best_card_score == handdisp->card_score[c])
								cairo_set_source_rgb (cr, HAND_DISPLAY_BEST_NEG_FONT);
							else
								cairo_set_source_rgb (cr, HAND_DISPLAY_NEG_FONT);
					}
					cairo_move_to (cr, x, y);
					cairo_show_text (cr, rank_str[c % 13]);
				}
				x += extents.x_advance; y += extents.y_advance;
			} else {
				handdisp->l[c] = handdisp->r[c] = handdisp->t[c] = handdisp->b[c] = 0;
			}
		}

		/* MODE_X */
		if (handdisp->mode == HAND_DISPLAY_MODE_HAND_X) {
			c = 52 + suit;
			x += 3;
			cairo_text_extents (cr, "x", &extents);
			handdisp->l[c] = x + extents.x_bearing;
			handdisp->r[c] = x + extents.x_bearing + extents.width;
			handdisp->t[c] = y + extents.y_bearing;
			handdisp->b[c] = y + extents.y_bearing + extents.height;
			if (c == handdisp->cur_focus) {
				cairo_set_source_rgb (cr, HAND_DISPLAY_FOCUS_BG);
				cairo_rectangle (cr, handdisp->l[c] - 1, handdisp->t[c] - 1,
					extents.width + 2, extents.height + 2);
				cairo_fill (cr);
			}
			cairo_set_source_rgb (cr, HAND_DISPLAY_FONT);
			cairo_move_to (cr, x, y);
			cairo_show_text (cr, "x");
			x += extents.x_advance + 4;
		}

		if (x > handdisp->want_width) { /* grow window */
			handdisp->want_width = x;
			gtk_widget_queue_resize (hand);
		}
	}

	/* show card scores */
	cairo_set_font_size (cr, 10);
	int c;
	for (c = 51; c >= 0; c--) {
		char *buf;
		if (handdisp->card_score[c] != HAND_DISPLAY_NO_SCORE) {
			buf = overtricks (handdisp->card_score[c]);
			cairo_text_extents (cr, buf, &extents);
			cairo_move_to (cr, handdisp->r[c] - extents.x_advance, handdisp->b[c]);
			cairo_set_source_rgb (cr, HAND_DISPLAY_DD_FONT);
			cairo_show_text (cr, buf);
		}
	}
}

static void
redraw_card (GtkWidget *hand, int card)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	GdkRectangle rect;
	rect.x = handdisp->l[card] - 2;
	rect.y = handdisp->t[card] - 7;
	rect.width = handdisp->r[card] - handdisp->l[card] + 4;
	rect.height = handdisp->b[card] - handdisp->t[card] + 8;
	gdk_window_invalidate_rect (gtk_widget_get_window (hand), &rect, FALSE);
}

/* private callbacks */

static gboolean
hand_display_motion (GtkWidget *hand, GdkEventMotion *event)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	int card = which_card(handdisp, event->x, event->y);
	if (handdisp->drag && handdisp->cur_click >= 0 && handdisp->cur_click < 52 &&
		gtk_drag_check_threshold (hand, handdisp->drag_x, handdisp->drag_y, event->x, event->y))
	{
		if (! target_list)
			target_list = gtk_target_list_new (target_entry, 1);
		handdisp->cur_drag = handdisp->cur_click;
		gtk_drag_begin (hand, target_list, GDK_ACTION_COPY, 1, (GdkEvent *) event);
	}
	if (handdisp->cur_focus != card) {
		if (handdisp->cur_focus != -1) {
			redraw_card (hand, handdisp->cur_focus);
			g_signal_emit_by_name (handdisp, "card-leave", handdisp->cur_focus);
		}
		handdisp->cur_focus = card;
		if (card != -1) {
			redraw_card (hand, card);
			g_signal_emit_by_name (handdisp, "card-enter", card);
		}
	}
	gdk_window_get_pointer(gtk_widget_get_window (hand), NULL, NULL, NULL); /* request more pointer hints */
	return FALSE;
}

static gboolean
hand_display_leave (GtkWidget *hand, GdkEventCrossing *event)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	if (handdisp->cur_focus != -1) {
		redraw_card (hand, handdisp->cur_focus);
		g_signal_emit_by_name (handdisp, "card-leave", handdisp->cur_focus);
		handdisp->cur_focus = -1;
	}
	return FALSE;
}

static gboolean
hand_display_button_press (GtkWidget *hand, GdkEventButton *event)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	int card = which_card(handdisp, event->x, event->y);

	if (event->type != GDK_BUTTON_RELEASE) /* don't trigger after dnd */
		handdisp->cur_focus = card;
	if (event->type == GDK_BUTTON_PRESS) {
		handdisp->cur_click = card;
		if (handdisp->drag) {
			handdisp->drag_x = event->x;
			handdisp->drag_y = event->y;
		}
	}
	if (handdisp->cur_focus == -1)
		return FALSE;
	redraw_card (hand, card);

	if (event->type == GDK_BUTTON_RELEASE) {
		if (handdisp->cur_click == card)
			g_signal_emit_by_name (handdisp, "card-clicked", card);
		handdisp->cur_click = -1;
	}
	return FALSE;
}

static void
hand_display_realize (GtkWidget *widget)
{
	GdkWindowAttr attributes;
	gint attributes_mask;

	g_return_if_fail (widget != NULL);
	g_return_if_fail (IS_HAND_DISPLAY (widget));

	gtk_widget_set_realized (widget, TRUE);

	GtkAllocation allocation;
	gtk_widget_get_allocation (widget, &allocation);
	attributes.x = allocation.x;
	attributes.y = allocation.y;
	attributes.width = allocation.width;
	attributes.height = allocation.height;
	attributes.wclass = GDK_INPUT_OUTPUT;
	attributes.window_type = GDK_WINDOW_CHILD;
	attributes.event_mask = gtk_widget_get_events (widget) |
		GDK_EXPOSURE_MASK | GDK_LEAVE_NOTIFY_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK |
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK;
	attributes.visual = gtk_widget_get_visual (widget);
	attributes.colormap = gtk_widget_get_colormap (widget);

	attributes_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
	GdkWindow *window = gdk_window_new (gtk_widget_get_parent_window (widget), &attributes, attributes_mask);
	gtk_widget_set_window (widget, window);
	gtk_widget_style_attach (widget);

	gdk_window_set_user_data (window, widget);

	gtk_style_set_background (gtk_widget_get_style (widget), window, GTK_STATE_ACTIVE);
}

static gboolean
hand_display_expose (GtkWidget *hand, GdkEventExpose *event)
{
	cairo_t *cr;

	/* get a cairo_t */
	cr = gdk_cairo_create (gtk_widget_get_window (hand));

	/* set a clip region for the expose event */
	cairo_rectangle (cr, event->area.x, event->area.y,
			event->area.width, event->area.height);
	cairo_clip (cr);
	draw (hand, cr);
	cairo_destroy (cr);

	return FALSE;
}

static void
hand_display_size_request (GtkWidget *hand, GtkRequisition *requisition)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	requisition->width = handdisp->want_width;
	requisition->height = handdisp->mode != HAND_DISPLAY_MODE_CARD ? 90 : 10;
}

static void
hand_display_size_allocate (GtkWidget *hand, GtkAllocation *allocation)
{
	g_return_if_fail (hand != NULL);
	g_return_if_fail (IS_HAND_DISPLAY (hand));
	g_return_if_fail (allocation != NULL);

	gtk_widget_set_allocation (hand, allocation);
	if (gtk_widget_get_realized (hand)) {
		gdk_window_move_resize (gtk_widget_get_window (hand),
				allocation->x, allocation->y,
				allocation->width, allocation->height);
	}
}

/* drag-and-drop interface */
/* http://live.gnome.org/GnomeLove/DragNDropTutorial */

static void
hand_display_drag_begin (GtkWidget *hand, GdkDragContext *dc, gpointer data /* unused */)
{
	HandDisplay *handdisp = HAND_DISPLAY (hand);
	assert (handdisp->cur_drag >= 0 && handdisp->cur_drag < 52);
	handdisp->cards[handdisp->cur_drag] |= HAND_DISPLAY_INVISIBLE_CARD;
	handdisp->cur_click = -1;
	redraw_card (hand, handdisp->cur_drag);

	/* create drag widget */
	if (drag_win) { /* should be NULL, but happens sometimes */
		gtk_widget_destroy (drag_win);
		drag_win = NULL;
	}
	drag_win = gtk_window_new (GTK_WINDOW_POPUP);
	GtkWidget *card = hand_display_new (HAND_DISPLAY_MODE_CARD);
	hand_display_set_style (HAND_DISPLAY (card), handdisp->style);
	hand_display_card_set_card (HAND_DISPLAY (card), handdisp->cur_drag);
	gtk_container_add (GTK_CONTAINER (drag_win), card);
	gtk_drag_set_icon_widget (dc, drag_win, 0, 0);
	gtk_widget_show_all (drag_win);
}

static gboolean
hand_display_drag_motion (GtkWidget *hand, GdkDragContext *dc,
        gint x, gint y, guint t, gpointer data)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	int card = which_card(handdisp, x, y);
	if (handdisp->cur_focus != card) {
		if (handdisp->cur_focus != -1) {
			redraw_card (hand, handdisp->cur_focus);
			g_signal_emit_by_name (handdisp, "card-drag-leave", handdisp->cur_focus);
		}
		handdisp->cur_focus = card;
		if (card != -1) {
			redraw_card (hand, card);
			g_signal_emit_by_name (handdisp, "card-drag-enter", card);
		}
	}
	return FALSE;
}

static void
hand_display_drag_leave (GtkWidget *hand, GdkDragContext *dc, gpointer data)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	if (handdisp->cur_focus != -1) {
		redraw_card (hand, handdisp->cur_focus);
		g_signal_emit_by_name (handdisp, "card-drag-leave", handdisp->cur_focus);
		handdisp->cur_focus = -1;
	}
}

static gboolean
hand_display_drag_drop (GtkWidget *hand, GdkDragContext *dc, gpointer data)
{
	gtk_drag_get_data (hand, dc, 0, 0);
	return TRUE;
}

static void
hand_display_drag_data_get (GtkWidget *hand, GdkDragContext *dc,
        GtkSelectionData *selection_data, guint targettype, guint t, gpointer data)
{
	HandDisplay *handdisp = HAND_DISPLAY (hand);
	if (handdisp->cur_drag < 0)
		return;
	assert (targettype == 0);
	gtk_selection_data_set (selection_data, gtk_selection_data_get_target (selection_data),
			32, (guchar *) &(handdisp->cur_drag), sizeof (int));
}

static void
hand_display_drag_data_received (GtkWidget *hand, GdkDragContext *dc,
        gint x, gint y, GtkSelectionData *selection_data,
        guint targettype, guint t, gpointer data)
{
	HandDisplay *handdisp = HAND_DISPLAY (hand);
	int *card = (int *) gtk_selection_data_get_data (selection_data);
	int on_card = which_card(handdisp, x, y);
	/*
	if (*card == on_card) {
		gtk_drag_finish (dc, FALSE, FALSE, t);
		return;
	}
	*/
	g_signal_emit_by_name (handdisp, "card-drag-drop", *card, on_card);
	gtk_drag_finish (dc, TRUE, FALSE, t);
}

/*
static void
hand_display_drag_data_delete (GtkWidget *hand, GdkDragContext *dc, gpointer data)
{
	printf ("drag_data_delete %x %x %x\n", hand, dc, data);
}
*/

static void
hand_display_drag_end (GtkWidget *hand, GdkDragContext *dc, gpointer data)
{
	HandDisplay *handdisp = HAND_DISPLAY (hand);
	if (handdisp->cur_drag >= 0) {
		handdisp->cards[handdisp->cur_drag] &= ~HAND_DISPLAY_INVISIBLE_CARD;
		redraw_card (hand, handdisp->cur_drag);
	}
	handdisp->cur_drag = -1;
	gtk_widget_destroy (drag_win);
	drag_win = NULL;
}

static void
setup_dnd (HandDisplay *handdisp)
{
	GtkWidget *hand = GTK_WIDGET (handdisp);

	/* start manually
	gtk_drag_source_set(
		hand,
		GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
		target_entry,
		1,
		GDK_ACTION_COPY
	);
	*/
	gtk_drag_dest_set(
		hand,
		GTK_DEST_DEFAULT_MOTION |
		// GTK_DEST_DEFAULT_HIGHLIGHT |
		GTK_DEST_DEFAULT_DROP,
		target_entry,
		1,
		GDK_ACTION_COPY
	);

	g_signal_connect (GTK_OBJECT(hand), "drag-begin",
			GTK_SIGNAL_FUNC(hand_display_drag_begin), NULL);
	g_signal_connect (GTK_OBJECT(hand), "drag-leave",
			GTK_SIGNAL_FUNC(hand_display_drag_leave), NULL);
	g_signal_connect (GTK_OBJECT(hand), "drag-motion",
			GTK_SIGNAL_FUNC(hand_display_drag_motion), NULL);
	g_signal_connect (GTK_OBJECT(hand), "drag-drop",
			GTK_SIGNAL_FUNC(hand_display_drag_drop), NULL);
	g_signal_connect (GTK_OBJECT(hand), "drag-data-get",
			GTK_SIGNAL_FUNC(hand_display_drag_data_get), NULL);
	g_signal_connect (GTK_OBJECT(hand), "drag-data-received",
			GTK_SIGNAL_FUNC(hand_display_drag_data_received), NULL);
	//g_signal_connect (GTK_OBJECT(hand), "drag_data_delete",
			//GTK_SIGNAL_FUNC(hand_display_drag_data_delete), NULL);
	g_signal_connect (GTK_OBJECT(hand), "drag-end",
			GTK_SIGNAL_FUNC(hand_display_drag_end), NULL);
}

/* initializers */

static void
hand_display_class_init (HandDisplayClass *class)
{
	//GtkObjectClass *object_class;
	GtkWidgetClass *widget_class;

	//object_class = GTK_OBJECT_CLASS (class);
	widget_class = GTK_WIDGET_CLASS (class);

	//parent_class = gtk_type_class (gtk_widget_get_type ());

	//object_class->destroy = hand_display_destroy;

	widget_class->realize = hand_display_realize;
	widget_class->expose_event = hand_display_expose;
	widget_class->size_request = hand_display_size_request;
	widget_class->size_allocate = hand_display_size_allocate;
	widget_class->motion_notify_event = hand_display_motion;
	widget_class->leave_notify_event = hand_display_leave;
	widget_class->button_press_event = hand_display_button_press;
	widget_class->button_release_event = hand_display_button_press;

	g_signal_new ("card-clicked",
			TYPE_HAND_DISPLAY,
			G_SIGNAL_RUN_LAST,
			0 /* guint class_offset */,
			NULL /* GSignalAccumulator accumulator */,
			NULL /* gpointer accu_data */,
			g_cclosure_marshal_VOID__INT /* GSignalCMarshaller c_marshaller */,
			G_TYPE_NONE /* GType return_type */,
			1 /* guint n_params */,
			G_TYPE_INT);
	g_signal_new ("card-enter", TYPE_HAND_DISPLAY, G_SIGNAL_RUN_LAST, 0, NULL, NULL,
			g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
	g_signal_new ("card-leave", TYPE_HAND_DISPLAY, G_SIGNAL_RUN_LAST, 0, NULL, NULL,
			g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
	g_signal_new ("card-drag-enter", TYPE_HAND_DISPLAY, G_SIGNAL_RUN_LAST, 0, NULL, NULL,
			g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
	g_signal_new ("card-drag-leave", TYPE_HAND_DISPLAY, G_SIGNAL_RUN_LAST, 0, NULL, NULL,
			g_cclosure_marshal_VOID__INT, G_TYPE_NONE, 1, G_TYPE_INT);
	g_signal_new ("card-drag-drop", TYPE_HAND_DISPLAY, G_SIGNAL_RUN_LAST, 0, NULL, NULL,
			g_cclosure_user_marshal_VOID__INT_INT, G_TYPE_NONE, 2, G_TYPE_INT, G_TYPE_INT);
}

static void
hand_display_init (HandDisplay *handdisp)
{
	int i;
	handdisp->style = HAND_DISPLAY_STYLE_TEXT;

	if (handdisp->mode == HAND_DISPLAY_MODE_TABLE) {
		for (i = 0; i < 4; i++) {
			handdisp->table_seat[i] = handdisp->table_card[i] = 0;
		}
		return;
	}

	int suitlist[] = { 3, 2, 0, 1 }; /* S H C D */
	for (i = 0; i < 4; i++)
		handdisp->suits[i] = suitlist[i];

	for (i = 0; i < 52; i++) {
		handdisp->cards[i] = 0;
		handdisp->card_score[i] = -1;
		handdisp->l[i] = handdisp->r[i] = handdisp->t[i] = handdisp->b[i] = -1;
	}
	handdisp->card_score_neg = 0;
	handdisp->best_card_score = HAND_DISPLAY_NO_SCORE;

	handdisp->cur_focus = handdisp->cur_click = handdisp->cur_drag = -1;
}

/* public interface */

GType
hand_display_get_type ()
{
	static GType hand_display_type = 0;

	if (!hand_display_type)
	{
		static const GTypeInfo hand_display_info = {
			sizeof (HandDisplayClass),
			NULL, /* base_init */
			NULL, /* base_finalize */
			(GClassInitFunc) hand_display_class_init,
			NULL, /* class_finalize */
			NULL, /* class_data */
			sizeof (HandDisplay),
			0,    /* n_preallocs */
			(GInstanceInitFunc) hand_display_init,
		};

		hand_display_type = g_type_register_static (GTK_TYPE_WIDGET,
				"HandDisplay",
				&hand_display_info,
				0);
	}

	return hand_display_type;
}

GtkWidget *
hand_display_new (int mode)
{
	HandDisplay *handdisp = g_object_new (TYPE_HAND_DISPLAY, NULL);
	handdisp->mode = mode;
	handdisp->want_width = mode != HAND_DISPLAY_MODE_CARD ? 100 : 10;
	return GTK_WIDGET(handdisp);
}

void
hand_display_draw (GtkWidget *hand)
{
	GtkAllocation allocation;
	gtk_widget_get_allocation (hand, &allocation);
	gtk_widget_queue_draw_area (hand, 0, 0, allocation.width, allocation.height);
}

void
hand_display_set_style (HandDisplay *handdisp, int style)
{
	handdisp->style = style;
}

void
hand_display_set_svg (char *fname, int width)
{
	assert (fname);
	assert (width > 0);
	if (svg_filename && ! strcmp (fname, svg_filename) && width == card_width)
		return; /* no change */

	if (svg_filename)
		g_free (svg_filename);
	svg_filename = strdup (fname);
	card_width = width;

	render_card_init (svg_filename);
}

void
hand_display_set_drag (HandDisplay *handdisp, int drag)
{
	handdisp->drag = drag;
	if (drag) {
		setup_dnd (handdisp);
	}
}

/* hand interface */
void
hand_display_set_card (HandDisplay *handdisp, int card, int val)
{
	assert (card >= 0 && card < 52);
	handdisp->cards[card] = val;
	handdisp->card_score[card] = HAND_DISPLAY_NO_SCORE;
}

void
hand_display_set_card_score (HandDisplay *handdisp, int card, int score)
{
	assert (card >= 0 && card < 52);
	handdisp->card_score[card] = score;
}

void
hand_display_set_card_score_neg (HandDisplay *handdisp, int neg)
{
	handdisp->card_score_neg = neg;
	handdisp->best_card_score = HAND_DISPLAY_NO_SCORE;
}

/* table interface */
void
hand_display_table_reset_cards (HandDisplay *handdisp)
{
	int i;
	for (i = 0; i < 4; i++) {
		handdisp->table_seat[i] = handdisp->table_card[i] = 0;
	}
}

void
hand_display_table_set_card (HandDisplay *handdisp, int n, int seat, int card)
{
	assert (card >= 0 && card < 52);
	handdisp->table_seat[n] = seat;
	handdisp->table_card[n] = card;
}

/* card interface */
void
hand_display_card_set_card (HandDisplay *handdisp, int card)
{
	assert (card >= 0 && card < 52);
	handdisp->table_card[0] = card;
}

