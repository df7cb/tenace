#ifndef HAND_DISPLAY_H
#define HAND_DISPLAY_H

#include <gdk/gdk.h>
#include <gtk/gtkwidget.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TYPE_HAND_DISPLAY          (hand_display_get_type ())
#define HAND_DISPLAY(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_HAND_DISPLAY, HandDisplay))
#define HAND_DISPLAY_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), HAND_DISPLAY,  HandDisplayClass))
#define IS_HAND_DISPLAY(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_HAND_DISPLAY))
//#define IS_HAND_DISPLAY_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((obj), TYPE_HAND_DISPLAY))
//#define HAND_DISPLAY_GET_CLASS     (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_HAND_DISPLAY, HandDisplayClass))

#define HAND_DISPLAY_NO_SCORE      (0x1000) /* score undefined */

typedef struct _HandDisplay            HandDisplay;
typedef struct _HandDisplayClass       HandDisplayClass;

struct _HandDisplay
{
	GtkDrawingArea parent;
	int cur_focus;
	int cur_click;
	int cards[52];
	int card_score[52];
	int card_score_neg[52];
	double l[52], r[52], t[52], b[52];

       /* private */
};

struct _HandDisplayClass
{
       GtkDrawingAreaClass parent_class;
};

GtkType hand_display_get_type (void);
GtkWidget * hand_display_new (void);
void hand_display_draw (GtkWidget *hand);
void hand_display_set_card (HandDisplay *handdisp, int card, int val);
void hand_display_set_card_score (HandDisplay *handdisp, int card, int score, int neg);

gboolean DNDDragMotionCB(
        GtkWidget *hand, GdkDragContext *dc,
        gint x, gint y, guint t,
        gpointer data
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* HAND_DISPLAY_H */
