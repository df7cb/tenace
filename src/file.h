GString *board_format_line(board *b, char handsep, char suitsep);
void board_save(board *b);
int board_parse_line(const char *line, board *b, char handsep, char suitsep);
int board_load(char *fname, board *b);

