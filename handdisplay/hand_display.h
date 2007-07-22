/*
 *  hand display - bridge hand widget for GTK+
 *  Copyright (C) 2007 Christoph Berg <cb@df7cb.de>
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

#ifndef HAND_DISPLAY_H
#define HAND_DISPLAY_H

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TYPE_HAND_DISPLAY          (hand_display_get_type ())
#define HAND_DISPLAY(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_HAND_DISPLAY, HandDisplay))
//#define HAND_DISPLAY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_HAND_DISPLAY,  HandDisplayClass))
#define IS_HAND_DISPLAY(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_HAND_DISPLAY))

/* display modes */
#define HAND_DISPLAY_MODE_HAND     0
#define HAND_DISPLAY_MODE_HAND_X   1
#define HAND_DISPLAY_MODE_TABLE    2

/* display styles */
#define HAND_DISPLAY_STYLE_TEXT    0
#define HAND_DISPLAY_STYLE_CARDS   1

#define HAND_DISPLAY_NO_CARD       0
#define HAND_DISPLAY_CARD          1
#define HAND_DISPLAY_GREY_CARD     2
#define HAND_DISPLAY_OLD_CARD      3

#define HAND_DISPLAY_NO_SCORE      (0x1000) /* score undefined */

/* internally used colors */
#define HAND_DISPLAY_TABLE_BG      0.0, 0.6, 0.0
#define HAND_DISPLAY_TABLE_GDK_BG  0.0*65535, 0.6*65535, 0.0*65535
#define HAND_DISPLAY_FOCUS_BG      1.0, 0.8, 0.0

#define HAND_DISPLAY_FONT          0.0, 0.0, 0.0
#define HAND_DISPLAY_GREY_FONT     0.2, 0.2, 0.2
#define HAND_DISPLAY_OLD_FONT      0.5, 0.5, 0.5
#define HAND_DISPLAY_BEST_POS_FONT 0.0, 0.9, 0.0
#define HAND_DISPLAY_POS_FONT      0.0, 0.7, 0.0
#define HAND_DISPLAY_BEST_NEG_FONT 0.9, 0.0, 0.0
#define HAND_DISPLAY_NEG_FONT      0.7, 0.0, 0.0
#define HAND_DISPLAY_DD_FONT       0.0, 0.0, 1.0

#define HAND_DISPLAY_SPADES_FONT   0.0, 0.0, 0.7
#define HAND_DISPLAY_HEARTS_FONT   0.9, 0.0, 0.0
#define HAND_DISPLAY_DIAMONDS_FONT 1.0, 0.3, 0.0
#define HAND_DISPLAY_CLUBS_FONT    0.0, 0.3, 0.0

typedef struct _HandDisplay            HandDisplay;
typedef struct _HandDisplayClass       HandDisplayClass;

struct _HandDisplay
{
	GtkDrawingArea parent;
	int mode;
	int style;

	/* hand */
	int cur_focus;
	int cur_click;
	int cards[52];
	int card_score[52];
	int card_score_neg;
	int best_card_score;
	double l[56], r[56], t[56], b[56]; /* 52..55 are MODE_X */
	int suits[4]; /* order in which to show suits */

	/* table */ // FIXME: this should use a union or something
	int table_seat[4];
	int table_card[4];
};

struct _HandDisplayClass
{
       GtkDrawingAreaClass parent_class;
};

GtkType hand_display_get_type (void);
GtkWidget *hand_display_new (int mode);
void hand_display_draw (GtkWidget *hand);
void hand_display_set_style (HandDisplay *handdisp, int style, char *fname);

/* hand interface */
void hand_display_set_card (HandDisplay *handdisp, int card, int val);
void hand_display_set_card_score (HandDisplay *handdisp, int card, int score);
void hand_display_set_card_score_neg (HandDisplay *handdisp, int neg);

/* table interface */
void hand_display_table_reset_cards (HandDisplay *handdisp);
void hand_display_table_set_card (HandDisplay *handdisp, int n, int seat, int card);

gboolean DNDDragMotionCB(
        GtkWidget *hand, GdkDragContext *dc,
        gint x, gint y, guint t,
        gpointer data
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAND_DISPLAY_H */
