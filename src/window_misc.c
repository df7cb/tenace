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

#include <gtk/gtk.h>

#include "window_board.h"


/* IMP table */

static GtkWidget *window_imps = NULL;

void
on_imp_table1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (!window_imps) {
		window_imps = glade_xml_get_widget (win->xml, "window_imps");
		gtk_widget_show (window_imps);
	}
}


void
on_imps_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	if (window_imps) {
		gtk_widget_hide (GTK_WIDGET(window_imps));
		window_imps = NULL;
	}
}


gboolean
on_window_imps_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	window_imps = NULL;
	return FALSE;
}


/* about dialog */

static GtkWidget *window_info = NULL;

void
on_info1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (!window_info) {
		window_info = glade_xml_get_widget (win->xml, "aboutdialog1");
		gtk_widget_show (window_info);
	}
}


void
on_aboutdialog1_close                  (GtkDialog       *dialog,
                                        gpointer         user_data)
{
	if (window_info) {
		gtk_widget_hide (GTK_WIDGET(window_info));
		window_info = NULL;
	}
}


void
on_aboutdialog1_response               (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
	if (window_info) {
		gtk_widget_hide (GTK_WIDGET(window_info));
		window_info = NULL;
	}
}


gboolean
on_aboutdialog1_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	window_info = NULL;
	return FALSE;
}

