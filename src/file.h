GString *board_format_line(board *b, char handsep, char suitsep);
void board_save(board *b, char *filename);
int board_parse_line(const char *line, board *b, char handsep, char suitsep);
int board_load(char *fname, board *b);
void board_load_dialog (board *b);
void board_save_dialog (board *b, int save_as);

