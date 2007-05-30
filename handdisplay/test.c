#include <gtk/gtk.h>

#include "hand_display.h"

/* DND specific event/signal callbacks. */
static void DNDBeginCB(
	GtkWidget *widget, GdkDragContext *dc, gpointer data
)
{
}
static void DNDEndCB(
	GtkWidget *widget, GdkDragContext *dc, gpointer data
)
{
}
static gboolean DNDDragMotionCB(
        GtkWidget *widget, GdkDragContext *dc,
        gint x, gint y, guint t,
        gpointer data
)
{
}
static void DNDDataRequestCB(
        GtkWidget *widget, GdkDragContext *dc,
        GtkSelectionData *selection_data, guint info, guint t,
        gpointer data
)
{
}
static void DNDDataRecievedCB(
        GtkWidget *widget, GdkDragContext *dc,
        gint x, gint y, GtkSelectionData *selection_data,  
        guint info, guint t, gpointer data
)
{
}
static void DNDDataDeleteCB(
	GtkWidget *widget, GdkDragContext *dc, gpointer data
)
{
}



void clicked (GtkWidget *instance, int *i, char *data)
{
	printf("%s %c%d clicked (callback)\n", data, "CDHS"[*i / 13], *i % 13 + 2);
}


static void
setup_dnd(GtkWidget *hand, GtkWidget *window)
{
	GtkTargetEntry target_entry[3];

		/* DND: Set up the clist as a potential DND destination.
		 * First we set up target_entry which is a sequence of of
		 * structure which specify the kinds (which we define) of
		 * drops accepted on this widget.
		 */

		/* Set up the list of data format types that our DND
		 * callbacks will accept.
		 */
		target_entry[0].target = "card";
		target_entry[0].flags = 0;
		target_entry[0].info = 0;
                target_entry[1].target = "foo";
                target_entry[1].flags = 0;
                target_entry[1].info = 1;
                target_entry[2].target = "bla";
                target_entry[2].flags = 0;
                target_entry[2].info = 2;

		/* Set the drag destination for this widget, using the
		 * above target entry types, accept move's and coppies'.
		 */
		gtk_drag_dest_set(
			hand,
			GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_HIGHLIGHT |
			GTK_DEST_DEFAULT_DROP,
			target_entry,
			sizeof(target_entry) / sizeof(GtkTargetEntry),
			GDK_ACTION_MOVE | GDK_ACTION_COPY
		);
		gtk_signal_connect(
			GTK_OBJECT(hand), "drag_motion",
			GTK_SIGNAL_FUNC(DNDDragMotionCB),
			window
		);

		/* Set the drag source for this widget, allowing the user
		 * to drag items off of this clist.
		 */
		gtk_drag_source_set(
			hand,
			GDK_BUTTON1_MASK | GDK_BUTTON2_MASK,
                        target_entry,
                        sizeof(target_entry) / sizeof(GtkTargetEntry),
			GDK_ACTION_MOVE | GDK_ACTION_COPY
		);
		/* Set DND signals on clist. */
		gtk_signal_connect(
			GTK_OBJECT(hand), "drag_begin",
			GTK_SIGNAL_FUNC(DNDBeginCB), window
		);
                gtk_signal_connect(
                        GTK_OBJECT(hand), "drag_end",
                        GTK_SIGNAL_FUNC(DNDEndCB), window
                );
                gtk_signal_connect(
                        GTK_OBJECT(hand), "drag_data_get",
                        GTK_SIGNAL_FUNC(DNDDataRequestCB), window
                );
                gtk_signal_connect(
                        GTK_OBJECT(hand), "drag_data_received",
                        GTK_SIGNAL_FUNC(DNDDataRecievedCB), window
                );
                gtk_signal_connect(
                        GTK_OBJECT(hand), "drag_data_delete",
                        GTK_SIGNAL_FUNC(DNDDataDeleteCB), window
                );

}

int
 main (int argc, char **argv)
 {
        GtkWidget *window;
        GtkWidget *hand;
	GtkWidget *table;

        gtk_init (&argc, &argv);

        window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        g_signal_connect (window, "destroy",
                        G_CALLBACK (gtk_main_quit), NULL);

	table = gtk_table_new (4, 4, FALSE);

        gtk_container_add (GTK_CONTAINER (window), table);


	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 3, 4, 0, 1); /* W */
	hand_display_set_card(HAND_DISPLAY(hand), 1, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 2, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 3, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "W");

	setup_dnd (hand, window);

	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 0, 1, 0, 1); /* N */
	hand_display_set_card(HAND_DISPLAY(hand), 21, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 22, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 23, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "N");

	setup_dnd (hand, window);

	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 1, 2, 0, 1); /* S */
	hand_display_set_card(HAND_DISPLAY(hand), 48, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 39, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 49, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "S");

	setup_dnd (hand, window);

	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 2, 3, 0, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 12, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 16, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 36, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "E");

	setup_dnd (hand, window);

        gtk_widget_show_all (window);

        gtk_main ();
 }
