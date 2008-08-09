void window_play_init (window_board_t *win, board *b);
void window_play_update (board *b);
void window_play_delete (void);
gboolean
on_window_play_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

