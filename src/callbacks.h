#include <gtk/gtk.h>


void
on_neu1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on___ffnen1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_speichern1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_speichern_unter1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_beenden1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ausschneiden1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_kopieren1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_einf__gen1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_l__schen1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_info1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_window_hand_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_radiotoolbutton_west_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_radiotoolbutton_north_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_radiotoolbutton_east_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_radiotoolbutton_south_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data);
