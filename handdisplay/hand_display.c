#include <assert.h>
#include <glib/gtypes.h>
#include <gtk/gtk.h>

#include <math.h> /* M_PI */

#include "hand_display.h"

G_DEFINE_TYPE (HandDisplay, hand_display, GTK_TYPE_DRAWING_AREA);

static int
which_card (HandDisplay *handdisp, double x, double y)
{
	int c;
	for (c = 0; c < 52; c++) {
		if (handdisp->cards[c] && x >= handdisp->l[c] && x <= handdisp->r[c]
				       && y >= handdisp->t[c] && y <= handdisp->b[c]) {
			return c;
			break;
		}
	}
	return -1;
}

static void
draw (GtkWidget *hand, cairo_t *cr)
{
	double l, r, t, b, x, y;
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	cairo_text_extents_t extents;

	l = hand->allocation.x;
	r = l + hand->allocation.width;
	t = hand->allocation.y;
	b = t + hand->allocation.height;

	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
			CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size (cr, 20);

	//printf ("rendering %x %f %f %f %f\n", hand, l, r, t, b);
	//char *suit_str[] = {"♣", "♦", "♥", "♠"};
	char *suit_str[] = {"C", "D", "H", "S"};
	char *rank_str[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
	int suit;
	for (suit = 0; suit < 4; suit++) {
		x = 4;
		y = b - ((double) hand->allocation.height * (suit + 0.2) / 4.0);
		cairo_move_to (cr, x, y);
		cairo_text_extents (cr, suit_str[suit], &extents);
		x += extents.x_advance; y += extents.y_advance;
		cairo_show_text (cr, suit_str[suit]);

		int c;
		for (c = 13 * (suit + 1) - 1; c >= 13 * suit; c--) {
			if (handdisp->cards[c]) {
				cairo_text_extents (cr, rank_str[c % 13], &extents);
				handdisp->l[c] = x + extents.x_bearing;
				handdisp->r[c] = x + extents.x_bearing + extents.width;
				handdisp->t[c] = y + extents.y_bearing;
				handdisp->b[c] = y + extents.y_bearing + extents.height;
				if (c == handdisp->cur_focus) {
					cairo_set_source_rgb (cr, 1.0, 0.0, 0.0);
					//printf ("ext x_b %f x_a %f w %f", extents.x_bearing, extents.x_advance, extents.width);
					//printf (" y_b %f y_a %f h %f", extents.y_bearing, extents.y_advance, extents.height);
					//printf (" l %f r %f t %f b %f\n", handdisp->l[c],handdisp->r[c],handdisp->t[c],handdisp->b[c]);
					cairo_move_to (cr, handdisp->l[c], handdisp->b[c]);
					cairo_line_to (cr, handdisp->l[c], handdisp->t[c]);
					cairo_line_to (cr, handdisp->r[c], handdisp->t[c]);
					cairo_line_to (cr, handdisp->r[c], handdisp->b[c]);
					cairo_line_to (cr, handdisp->l[c], handdisp->b[c]);
					cairo_stroke (cr);
					cairo_move_to (cr, x, y);
				}
				cairo_show_text (cr, rank_str[c % 13]);
				x += extents.x_advance; y += extents.y_advance;
				if (c == handdisp->cur_focus)
					cairo_set_source_rgb (cr, 0.0, 0.0, 0.0);
			}
		}
	}
}

static void
redraw_card (GtkWidget *hand, int card)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	GdkRectangle rect;
	rect.x = handdisp->l[card] - 2;
	rect.y = handdisp->t[card] - 2;
	rect.width = handdisp->r[card] - handdisp->l[card] + 4;
	rect.height = handdisp->b[card] - handdisp->t[card] + 4;
	gdk_window_invalidate_rect (hand->window, &rect, FALSE);
}

static gboolean
hand_display_motion (GtkWidget *hand, GdkEventMotion *event)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	int card = which_card(handdisp, event->x, event->y);
	if (handdisp->cur_focus != card) {
		if (handdisp->cur_focus != -1) {
			redraw_card (hand, handdisp->cur_focus);
			printf("%c%d left\n", "CDHS"[handdisp->cur_focus / 13], handdisp->cur_focus % 13 + 2);
		}
		handdisp->cur_focus = card;
		if (card != -1) {
			redraw_card (hand, card);
			printf("%c%d entered\n", "CDHS"[card / 13], card % 13 + 2);
		}
	}
	gdk_window_get_pointer(hand->window, NULL, NULL, NULL); /* request more pointer hints */
	return FALSE;
}

static gboolean
hand_display_leave (GtkWidget *hand, GdkEventCrossing *event)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	if (handdisp->cur_focus != -1) {
		redraw_card (hand, handdisp->cur_focus);
		handdisp->cur_focus = -1;
		printf("%c%d left\n", "CDHS"[handdisp->cur_focus / 13], handdisp->cur_focus % 13 + 2);
	}
	return FALSE;
}

static gboolean
hand_display_button_press (GtkWidget *hand, GdkEventButton *event)
{
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	int card = which_card(handdisp, event->x, event->y);
	static int card_ptr[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12,
			13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
			26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
			39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

	handdisp->cur_focus = card;
	if (handdisp->cur_focus == -1)
		return;
	redraw_card (hand, card);

	printf("%c%d click type %d\n", "CDHS"[card / 13], card % 13 + 2, event->type);

	if (event->type == GDK_BUTTON_PRESS) {
		handdisp->cur_click = card;
	} else if (event->type == GDK_BUTTON_RELEASE && handdisp->cur_click == card) {
		g_signal_emit_by_name (HAND_DISPLAY(hand), "clicked", card_ptr + card);
	}
	return FALSE;
}

gboolean DNDDragMotionCB(
        GtkWidget *hand, GdkDragContext *dc,
        gint x, gint y, guint t,
        gpointer data
)
{
	printf ("DNDDragMotionCB %x %x %x x%d y%d t%d\n", hand, dc, data, x, y, t);
	HandDisplay *handdisp = HAND_DISPLAY(hand);
	int card = which_card(handdisp, x, y);
	handdisp->cur_focus = card;
	if (card != -1)
		redraw_card (hand, card);
}

static gboolean
hand_display_expose (GtkWidget *hand, GdkEventExpose *event)
{
	cairo_t *cr;

	/* get a cairo_t */
	cr = gdk_cairo_create (hand->window);

	/* set a clip region for the expose event */
	cairo_rectangle (cr, event->area.x, event->area.y,
			event->area.width, event->area.height);
	cairo_clip (cr);
	draw (hand, cr);
	cairo_destroy (cr);

	return FALSE;
}

static void
hand_display_class_init (HandDisplayClass *class)
{
	GtkWidgetClass *widget_class;

	widget_class = GTK_WIDGET_CLASS (class);

	widget_class->expose_event = hand_display_expose;
	widget_class->motion_notify_event = hand_display_motion;
	widget_class->leave_notify_event = hand_display_leave;
	widget_class->button_press_event = hand_display_button_press;
	widget_class->button_release_event = hand_display_button_press;

	g_signal_new ("clicked",
			TYPE_HAND_DISPLAY,
			G_SIGNAL_RUN_LAST,
			0 /* guint class_offset */,
			NULL /* GSignalAccumulator accumulator */,
			NULL /* gpointer accu_data */,
			g_cclosure_marshal_VOID__INT /* GSignalCMarshaller c_marshaller */,
			G_TYPE_NONE /* GType return_type */,
			1 /* guint n_params */,
			G_TYPE_INT);
}

static void
hand_display_init (HandDisplay *hand)
{
	int i;
	for (i = 0; i < 52; i++) {
		hand->cards[i] = 0;
		hand->l[i] = hand->r[i] = hand->t[i] = hand->b[i] = -1;
	}

	hand->cur_focus = hand->cur_click = -1;

	gtk_widget_add_events (GTK_WIDGET(hand),
		GDK_POINTER_MOTION_MASK | GDK_POINTER_MOTION_HINT_MASK |
		GDK_LEAVE_NOTIFY_MASK |
		GDK_BUTTON_PRESS_MASK | GDK_BUTTON_RELEASE_MASK);
}

GtkWidget *
hand_display_new (void)
{
	return g_object_new (TYPE_HAND_DISPLAY, NULL);
}

void
hand_display_set_card (HandDisplay *hand, int card, int val)
{
	assert (card >= 0 && card < 52);
	hand->cards[card] = val;
}
