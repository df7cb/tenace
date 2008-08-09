#include "window_board.h"

/* hooks into options window */
void window_options_board_populate (void);
void window_options_board_list_populate (void);

/* rcfile handling */
int read_config (window_board_t *win);
int write_config (window_board_t *win);

/* callbacks */
void
on_options1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
void
on_options_cancel_clicked              (GtkButton       *button,
                                        gpointer         user_data);
void
on_options_apply_clicked               (GtkButton       *button,
                                        gpointer         user_data);
void
on_options_ok_clicked                  (GtkButton       *button,
                                        gpointer         user_data);
gboolean
on_window_options_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
