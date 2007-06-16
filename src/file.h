GString *board_format_line(board *b, char handsep, char suitsep);
void board_save(board *b, char *filename);
int board_parse_line(const char *line, board *b, char handsep, char suitsep);
board * board_load (char *fname);
board * board_load_dialog (void);
void board_save_dialog (board *b, int save_as);

