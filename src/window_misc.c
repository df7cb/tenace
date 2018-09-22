/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2018 Christoph Berg <cb@df7cb.de>
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
#include <gtk/gtk.h>
#include <dds/dll.h>

#include "config.h"
#include "functions.h"
#include "window_board.h"


/* IMP table */

static GtkWidget *window_imps = NULL;

G_MODULE_EXPORT void
on_imp_table1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	window_imps = get_widget ("window_imps");
	gtk_widget_show (window_imps);
}


G_MODULE_EXPORT void
on_imps_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_hide (GTK_WIDGET(window_imps));
}


/* about dialog */

static GtkWidget *window_info = NULL;

G_MODULE_EXPORT void
on_info1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	window_info = get_widget ("aboutdialog1");
#ifdef DDS_VERSION
	GString *comment = g_string_new (gtk_about_dialog_get_comments (GTK_ABOUT_DIALOG (window_info)));
	g_string_append_printf (comment, _("\nDDS version: %d.%d.%d"),
			DDS_VERSION / 10000, (DDS_VERSION / 100) % 100, DDS_VERSION % 100);
	gtk_about_dialog_set_comments (GTK_ABOUT_DIALOG (window_info), comment->str);
	g_string_free (comment, TRUE);
#endif
	gtk_widget_show (window_info);
}


G_MODULE_EXPORT void
on_aboutdialog1_close                  (GtkDialog       *dialog,
                                        gpointer         user_data)
{
	gtk_widget_hide (GTK_WIDGET(window_info));
}


G_MODULE_EXPORT void
on_aboutdialog1_response               (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data)
{
	gtk_widget_hide (GTK_WIDGET(window_info));
}

