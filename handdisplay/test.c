#include <gtk/gtk.h>

#include "hand_display.h"

void clicked (GtkWidget *instance, int *i, char *data)
{
	printf ("Callback clicked %d %s\n", *i, data);
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

	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 0, 1, 0, 1); /* N */
	hand_display_set_card(HAND_DISPLAY(hand), 21, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 22, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 23, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "N");

	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 1, 2, 0, 1); /* S */
	hand_display_set_card(HAND_DISPLAY(hand), 48, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 39, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 49, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "S");

	hand = hand_display_new ();
	gtk_table_attach_defaults (GTK_TABLE(table), hand, 2, 3, 0, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 12, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 16, 1);
	hand_display_set_card(HAND_DISPLAY(hand), 36, 1);

	g_signal_connect(hand, "clicked", G_CALLBACK(clicked), "E");

        gtk_widget_show_all (window);

        gtk_main ();
 }
