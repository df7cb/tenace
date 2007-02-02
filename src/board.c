#include <stdio.h>
#include <ctype.h>

#include "bridge.h"

GString *board_format_line(board *b, char handsep, char suitsep)
{
	int h;
	GString *out = g_string_new(NULL);

	for (h = 0; h < 4; h++) {
		int s;
		card *c = b->hands[h]->cards;
		for (s = spade; s >= club; s--) {
			while (*c >= 0 && SUIT(*c) == s) {
				g_string_append_printf(out, "%c", rank_char(RANK(*c++)));
			}
			if (s > club)
				g_string_append_printf(out, "%c", suitsep);
		}
		if (h < 3)
			g_string_append_printf(out, "%c", handsep);
	}
	return out;
}

void board_save(board *b)
{
	int h;
	printf("%s\n", b->name->str);

	for (h = 0; h < 4; h++) {
		printf("%s\n", b->hands[h]->name->str);
		printf("%s\n", hand_string(b->hands[h])->str);
	}
	//board_save_giblib(b);
}

int board_parse_line(char *line, board *b, char handsep, char suitsep)
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
			if (give_card(b, se, (su * 13) + ra) != 1)
				printf("could not give card?\n");
		} else {
			printf ("parse error at char %d: %s\n", c - line + 1, line);
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

/*
void board_gib(board *b)
{
	char *h_char[] = {"w", "n", "e", "s"};
	int h;
	FILE *f;
	if (!(f = fopen("dd", "w"))) {
		perror("dd");
		return;
	}
	for (h = 0; h < 4; h++) {
		fprintf(f, "%s ", h_char[h]);
		fprintf(f, "%s\n", gib_string(b->hands[h])->str);
	}
	fprintf(f, "s s\nca\n");
	fclose(f);
	if (!(f = fopen("dc", "w"))) {
		perror("dc");
		return;
	}
	fprintf(f, "-vd dd\n");
	fclose(f);
	system("wine /home/cb/projects/bridge/gib/zip/bridge.exe -i dc");
}
*/

void board_dds(board *b)
{
	FILE *f;
	GString *s;
	if (!(f = fopen("dd", "w"))) {
		perror("dd");
		return;
	}
	s = board_format_line(b, ' ', '.');
	fprintf(f, "%s\n", s->str);
	fclose(f);
	g_string_free(s, TRUE);
	system("dds dd");
}
