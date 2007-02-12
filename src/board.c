#include <ctype.h>
#include <stdio.h>
#include <stdlib.h> /* system */

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
	if (!(f = fopen("dd", "w"))) {
		perror("dd");
		return;
	}
	char tr[] = { 'c', 'd', 'h', 's', 'n'};
	char le[] = { 0, 'w', 'n', 'e', 's' };

	card fulldeal[52];
	int i;
	for (i = 0; i < 52; i++)
		fulldeal[i] = b->cards[i];
	for (i = 0; i < b->n_played_cards; i++)
		fulldeal[b->played_cards[i]] = b->played_cards_seat[i];

	seat leader = b->n_played_cards % 4 == 0 ? b->current_lead :
		b->played_cards_seat[b->n_played_cards - (b->n_played_cards % 4)];
	fprintf(f, "{ name=dd trumps=%c leader=%c ", tr[b->trumps], le[leader]);
	if (b->n_played_cards)
		fprintf(f, "played=");
	for (i = 0; i < b->n_played_cards; i++) {
		fprintf(f, "%c%c", tr[SUIT(b->played_cards[i])], tolower(rank_char(RANK(b->played_cards[i]))));
		if (i % 4 == 3)
			fprintf(f, "-");
		else
			fprintf(f, ".");
	}
	fprintf(f, "}\n");

	int h;
	int n_cards[4] = {0, 0, 0, 0};
	char hands[4][13];
	GString *out = g_string_new(NULL);

	for (i = 51; i >= 0; i--) {
		int h = fulldeal[i] - 1;
		hands[h][n_cards[h]++] = i;
	}

	for (h = 0; h < 4; h++) {
		int s;
		i = 0;
		for (s = spade; s >= club; s--) {
			while (i < n_cards[h] && SUIT(hands[h][i]) == s) {
				g_string_append_printf(out, "%c", rank_char(RANK(hands[h][i++])));
			}
			if (s > club)
				g_string_append_printf(out, "%c", '.');
		}
		if (h < 3)
			g_string_append_printf(out, "%c", ' ');
	}

	fprintf(f, "%s\n", out->str);
	fclose(f);
	g_string_free(out, TRUE);
	system("dds dd");
}
