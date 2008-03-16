/* rcfile handling */
void window_options_populate (GtkWidget *window_options);
void apply_options (GtkWidget *window_options);
int read_config (window_board_t *win);
int write_config (window_board_t *win);
