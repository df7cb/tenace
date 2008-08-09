void card_window_update (seat *cards);
void window_card_set_style (int style);
void window_card_init (int style);
gboolean
on_window_card_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
void
on_cards1_activate                     (GtkMenuItem     *menuitem,
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
void
on_toolbutton_card_wipe_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);
void
on_toolbutton_card_random_clicked      (GtkToolButton   *toolbutton,
                                        gpointer         user_data);
