#include <ctype.h>
#include <stdio.h>
#include <string.h>

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

// pn|Frederic,gm,Myon,mecky|st||md|4S27KAHKD49C3589TJ,S36H48AD358KC27QK,STJH259TJQD26TJQC,|rh||ah|Board 14|sv|o|mb|p|mb|p|mb|1D|mb|2H|mb|2S|mb|p|mb|p|mb|p|pg||pc|HK|pc|H4|pc|H2|pc|H6|pg||pc|CJ|pc|CK|pc|ST|pc|C4|pg||pc|H9|pc|H3|pc|S2|pc|H8|pg||pc|CT|pc|C2|pc|SJ|pc|C6|pg||pc|HQ|pc|H7|pc|S7|pc|HA|pg||pc|D9|pc|D3|pc|DT|pc|DA|pg||pc|S4|pc|SK|pc|S3|pc|H5|pg||pc|SA|pc|S6|pc|HT|pc|S5|pg||pc|D4|pc|DK|pc|D2|pc|D7|pg||pc|C7|pc|D6|pc|CA|pc|C3|pg||mc|6|
#define STRTOK strtok_r(NULL, "|\n", &saveptr)
int board_parse_lin(const char *line, board *b)
{
	char *l = strdup(line);
	char *saveptr;
	char *tok;
	int c = 0;
	for (tok = strtok_r(l, "|", &saveptr); tok; tok = STRTOK) {
		if (!strcmp(tok, "pn")) { /* SWNE */
			g_string_printf(b->hand_name[south-1], "%s", strtok_r(NULL, ",|", &saveptr));
			g_string_printf(b->hand_name[west-1], "%s", strtok_r(NULL, ",|", &saveptr));
			g_string_printf(b->hand_name[north-1], "%s", strtok_r(NULL, ",|", &saveptr));
			g_string_printf(b->hand_name[east-1], "%s", strtok_r(NULL, ",|", &saveptr));
		} else if (!strcmp(tok, "md")) {
			tok = STRTOK;
			char *c;
			seat se = south;
			suit su = spade;
			b->dealer = seat_mod(*tok - '0' - 1);
			for (c = tok + 1; *c; c++) {
				rank ra;
				switch (*c) {
					case 'S': su = spade; break;
					case 'H': su = heart; break;
					case 'D': su = diamond; break;
					case 'C': su = club; break;
					case ',': se = seat_mod(se + 1); break;
					default:
						if ((ra = parse_rank_char(*c)) >= 0) {
							if (add_card(b, se, (su * 13) + ra) != 1)
								return 0;
							break;
						}
						printf("Parse error: %s", tok);
						return 0;
				}
			}
			deal_random(b); /* compute east hand */
		} else if (!strcmp(tok, "ah")) {
			g_string_printf(b->name, "%s", STRTOK);
		} else if (!strcmp(tok, "sv")) {
			tok = STRTOK;
			switch (*tok) {
				case 'o': b->vuln[0] = 0; b->vuln[1] = 0; break;
				case 'n': b->vuln[0] = 1; b->vuln[1] = 0; break;
				case 'e': b->vuln[0] = 1; b->vuln[1] = 1; break;
				case 'b': b->vuln[0] = 1; b->vuln[1] = 1; break;
				default: printf("Unknown vulnerability: %s\n", tok);
			}
		} else if (!strcmp(tok, "mb")) {
			// TODO: parse bidding
			STRTOK;
		} else if (!strcmp(tok, "pc")) {
			tok = STRTOK;
			suit su;
			switch (*tok) {
				case 'S': su = spade; break;
				case 'H': su = heart; break;
				case 'D': su = diamond; break;
				case 'C': su = club; break;
			}
			rank ra = parse_rank_char(tok[1]);
			if (c < 52)
				b->played_cards[c++] = 13 * su + ra;
		} else if (!strcmp(tok, "st")) {
		} else if (!strcmp(tok, "rh")) {
		} else if (!strcmp(tok, "pg")) {
		} else if (!strcmp(tok, "mc")) {
			// TODO: parse claim
			STRTOK;
		} else {
			printf("Unknown token '%s'\n", tok);
		}
	}
	return 1;
}
#undef STRTOK

int board_parse_line(const char *line, board *b, char handsep, char suitsep)
{
	const char *c = line;
	seat se = west;
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
				return 0;
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
	char buf[1024];
	if (!(f = fopen(fname, "r"))) {
		perror(fname);
		return 0;
	}
	if (fgets(buf, 1023, f) == NULL)
		return 0;
	fclose(f);
	//printf("board is %s\n", buf);
	board_clear(b);
	if (!strncmp(buf, "pn", 2)) {
		return board_parse_lin(buf, b);
	} else {
		return board_parse_line(buf, b, ' ', '.');
	}
}


