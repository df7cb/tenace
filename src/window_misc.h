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

/* IMP table */

void
on_imp_table1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_imps_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

/* about dialog */

void
on_info1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_aboutdialog1_close                  (GtkDialog       *dialog,
                                        gpointer         user_data);

void
on_aboutdialog1_response               (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data);
