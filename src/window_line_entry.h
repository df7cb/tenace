void board_set_from_line_entry(board *b);
void line_entry_set_from_board(board *b);
void window_line_entry_init();

void
on_line_entry_ok_clicked               (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_line_entry_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_line_entry_activate                 (GtkEntry        *entry,
                                        gpointer         user_data);
