void window_bids_init (void);
void window_bids_delete (void);
void
on_bid_clear_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);
void
on_bid_undo_clicked                    (GtkToolButton   *toolbutton,
                                        gpointer         user_data);
void
on_bid_set_contract_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);
gboolean
on_window_bidding_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
gboolean
on_window_bids_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
