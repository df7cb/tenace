#ifndef HAND_DISPLAY_H
#define HAND_DISPLAY_H

typedef struct _HandDisplay            HandDisplay;
typedef struct _HandDisplayClass       HandDisplayClass;

struct _HandDisplay
{
	GtkDrawingArea parent;
	int cur_focus;
	int cur_click;
	int cards[52];
	double l[52], r[52], t[52], b[52];

       /* private */
};

struct _HandDisplayClass
{
       GtkDrawingAreaClass parent_class;
};

#define TYPE_HAND_DISPLAY          (hand_display_get_type ())
#define HAND_DISPLAY(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_HAND_DISPLAY, HandDisplay))
#define HAND_DISPLAY_CLASS(obj)    (G_TYPE_CHECK_CLASS_CAST ((obj), HAND_DISPLAY,  HandDisplayClass))
#define IS_HAND_DISPLAY(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_HAND_DISPLAY))
#define IS_HAND_DISPLAY_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((obj), TYPE_HAND_DISPLAY))
#define HAND_DISPLAY_GET_CLASS     (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_HAND_DISPLAY, HandDisplayClass))

GtkWidget * hand_display_new (void);
void hand_display_set_card (HandDisplay *handdisp, int card, int val);

gboolean DNDDragMotionCB(
        GtkWidget *hand, GdkDragContext *dc,
        gint x, gint y, guint t,
        gpointer data
);

#endif /* HAND_DISPLAY_H */
