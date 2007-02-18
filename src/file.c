#include <ctype.h>
#include <stdio.h>

#include "bridge.h"
#include "functions.h"

GString *board_format_line(board *b, char handsep, char suitsep)
{
	GString *out = g_string_new(NULL);

	int h;
	for (h = 1; h < 5; h++) {
		int c;
		for (c = 51; c >= 39; c--)
			if (b->dealt_cards[c] == h)
				g_string_append_printf(out, "%c", rank_char(RANK(c)));
		g_string_append_printf(out, "%c", suitsep);
		for (c = 38; c >= 26; c--)
			if (b->dealt_cards[c] == h)
				g_string_append_printf(out, "%c", rank_char(RANK(c)));
		g_string_append_printf(out, "%c", suitsep);
		for (c = 25; c >= 13; c--)
			if (b->dealt_cards[c] == h)
				g_string_append_printf(out, "%c", rank_char(RANK(c)));
		g_string_append_printf(out, "%c", suitsep);
		for (c = 12; c >= 0; c--)
			if (b->dealt_cards[c] == h)
				g_string_append_printf(out, "%c", rank_char(RANK(c)));

		if (h < 4)
			g_string_append_printf(out, "%c", handsep);
	}
	return out;
}

void board_save(board *b)
{
	int h;
	printf("%s\n", b->name->str);

	for (h = 1; h < 5; h++) {
		printf("%s\n", b->hand_name[h-1]->str);
		printf("%s\n", hand_string(b, h)->str);
	}
}

static int board_parse_line(char *line, board *b, char handsep, char suitsep)
{
	seat se = west;
	char *c = line;
	int su = spade;
	while (*c && *c != '\n') {
		rank ra;
		if (*c == suitsep) {
			su--;
			if (su < 0)
				return 0;
		} else if (*c == handsep) {
			se++;
			su = spade;
			if (se > south)
				return 0;
		} else if ((ra = parse_rank_char(*c)) >= 0) {
			if (add_card(b, se, (su * 13) + ra) != 1)
				printf("could not give card?\n");
		} else {
			printf ("parse error at char %ld: %s\n", c - line + 1, line);
			return 0;
		}
		c++;
	}
	return 1;
}

int board_load(char *fname, board *b)
{
	FILE *f;
	char buf[100];
	if (!(f = fopen(fname, "r"))) {
		perror(fname);
		return 0;
	}
	if (fgets(buf, 99, f) == NULL)
		return 0;
	fclose(f);
	//printf("board is %s\n", buf);
	board_clear(b);
	if (!board_parse_line(buf, b, ' ', '.'))
		return 0;
	return 1;
}


