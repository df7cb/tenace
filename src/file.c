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

static char *lin_card_string(board *b)
{
	static char out[39+16+4+1];
	int i = 0, h;
	for (h = 4; h != 3; h = (h == 4 ? 1 : h + 1)) {
		int c;
		out[i++] = 'S';
		for (c = 39; c < 52; c++)
			if (b->dealt_cards[c] == h) out[i++] = rank_char(RANK(c));
		out[i++] = 'H';
		for (c = 26; c < 39; c++)
			if (b->dealt_cards[c] == h) out[i++] = rank_char(RANK(c));
		out[i++] = 'D';
		for (c = 13; c < 26; c++)
			if (b->dealt_cards[c] == h) out[i++] = rank_char(RANK(c));
		out[i++] = 'C';
		for (c = 0; c < 13; c++)
			if (b->dealt_cards[c] == h) out[i++] = rank_char(RANK(c));
		out[i++] = ',';
	}
	return out;
}

static char *lin_bid(card bid)
{
	static char buf[3];
	if (bid == bid_pass)
		return "p";
	else if (bid == bid_x)
		return "d";
	else if (bid == bid_xx)
		return "r";
	sprintf(buf, "%d%c", LEVEL(bid), "CDHSN"[DENOM(bid)]);
	return buf;
}

// pn|Frederic,gm,Myon,mecky|st||md|4S27KAHKD49C3589TJ,S36H48AD358KC27QK,STJH259TJQD26TJQC,|rh||ah|Board 14|sv|o|mb|p|mb|p|mb|1D|mb|2H|mb|2S|mb|p|mb|p|mb|p|pg||pc|HK|pc|H4|pc|H2|pc|H6|pg||pc|CJ|pc|CK|pc|ST|pc|C4|pg||pc|H9|pc|H3|pc|S2|pc|H8|pg||pc|CT|pc|C2|pc|SJ|pc|C6|pg||pc|HQ|pc|H7|pc|S7|pc|HA|pg||pc|D9|pc|D3|pc|DT|pc|DA|pg||pc|S4|pc|SK|pc|S3|pc|H5|pg||pc|SA|pc|S6|pc|HT|pc|S5|pg||pc|D4|pc|DK|pc|D2|pc|D7|pg||pc|C7|pc|D6|pc|CA|pc|C3|pg||mc|6|
static int board_save_lin(board *b, const char *filename)
{
	FILE *f;
	int i;

	if (!(f = fopen(filename, "w"))) {
		perror(filename);
		return -1;
	}

	fprintf(f, "pn|%s,%s,%s,%s|", b->hand_name[south-1]->str, b->hand_name[west-1]->str,
		b->hand_name[north-1]->str, b->hand_name[east-1]->str);
	fprintf(f, "st||");
	fprintf(f, "md|%d%s|", seat_mod(b->dealer + 1), lin_card_string(b));
	fprintf(f, "rh||");
	fprintf(f, "ah|%s|", b->name->str);
	fprintf(f, "sv|%c|", b->vuln[0] ? (b->vuln[1] ? 'b' : 'n')
					: (b->vuln[1] ? 'e' : 'o'));
	for (i = 0; i < b->n_bids; i++)
		fprintf(f, "mb|%s|", lin_bid(b->bidding[i]));
	for (i = 0; i < 52; i++) {
		if (i % 4 == 0)
			fprintf(f, "pg||");
		card c = b->played_cards[i];
		if (c < 0)
			break;
		if (c == claim_rest) {
			fprintf(f, "mc|%d|", 0); // TODO: what to claim here?
			break;
		}
		fprintf(f, "pc|%c%c|", "CDHS"[SUIT(c)], rank_char(RANK(c)));
	}
	fprintf(f, "\n");
	fclose(f);
	return 1;
}

void board_save(board *b, char *filename)
{
	int i;
	if ((i = strlen(filename)) > 4) {
		if (!strcmp(filename + i - 4, ".lin")) {
			board_save_lin(b, filename);
		} else if (!strcmp(filename + i - 4, ".pbn")) {
			printf("pbn not yet implemented\n");
		} else {
			int h;
			printf("%s\n", b->name->str);

			for (h = 1; h < 5; h++) {
				printf("%s\n", b->hand_name[h-1]->str);
				printf("%s\n", hand_string(b, h)->str);
			}
		}
	}
}

// pn|Frederic,gm,Myon,mecky|st||md|4S27KAHKD49C3589TJ,S36H48AD358KC27QK,STJH259TJQD26TJQC,|rh||ah|Board 14|sv|o|mb|p|mb|p|mb|1D|mb|2H|mb|2S|mb|p|mb|p|mb|p|pg||pc|HK|pc|H4|pc|H2|pc|H6|pg||pc|CJ|pc|CK|pc|ST|pc|C4|pg||pc|H9|pc|H3|pc|S2|pc|H8|pg||pc|CT|pc|C2|pc|SJ|pc|C6|pg||pc|HQ|pc|H7|pc|S7|pc|HA|pg||pc|D9|pc|D3|pc|DT|pc|DA|pg||pc|S4|pc|SK|pc|S3|pc|H5|pg||pc|SA|pc|S6|pc|HT|pc|S5|pg||pc|D4|pc|DK|pc|D2|pc|D7|pg||pc|C7|pc|D6|pc|CA|pc|C3|pg||mc|6|
#define STRTOK strtok_r(NULL, "|\n\r", &saveptr)
int board_parse_lin(const char *line, board *b)
{
	char *l = strdup(line);
	char *saveptr;
	char *tok;
	int card_nr = 0;
	int contract = 0;
	int doubled = 0;
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
				int i;
				if ((i = parse_suit(*c)) != -1) {
					su = i;
				} else if ((i = parse_rank_char(*c)) != -1) {
					if (add_card(b, se, (su * 13) + i) != 1)
						return 0;
				} else if (*c == ',') {
					se = seat_mod(se + 1);
				} else {
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
			int bid = parse_bid(tok = STRTOK);
			if (bid == -1) {
				printf("Invalid bid %s\n", tok);
				return 0;
			}
			board_append_bid(b, bid);
			if (bid > bid_xx) {
				contract = bid;
				doubled = 0;
			} else if (bid == bid_x || bid == bid_xx) {
				doubled = bid;
			}
		} else if (!strcmp(tok, "an")) {
			printf("TODO: alerts not parsed yet\n");
			STRTOK;
		} else if (!strcmp(tok, "pc")) {
			int c = parse_card(tok = STRTOK);
			if (c == -1) {
				printf("Invalid card %s\n", tok);
				return 0;
			}
			if (card_nr < 52)
				b->played_cards[card_nr++] = c;
		} else if (!strcmp(tok, "st")) {
		} else if (!strcmp(tok, "rh")) {
		} else if (!strcmp(tok, "pg")) {
		} else if (!strcmp(tok, "mc")) {
			b->played_cards[card_nr] = claim_rest; // no card_nr increment here
			STRTOK;
		} else {
			printf("Unknown token '%s'\n", tok);
		}
	}
	if (b->played_cards[0] != -1)
		board_set_contract(b, LEVEL(contract), DENOM(contract),
			seat_mod(b->dealt_cards[b->played_cards[0]] + 3), doubled);
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
	if (!strncmp(buf, "pn", 2) || !strncmp(buf, "vg", 2)) {
		return board_parse_lin(buf, b);
	} else {
		return board_parse_line(buf, b, ' ', '.');
	}
}

void board_load_dialog (board *b)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File",
			b->win,
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (board_load(filename, b)) {
			if (b->filename)
				g_string_free(b->filename, TRUE);
			b->filename = g_string_new(filename);
			card_window_update(b->dealt_cards);
			show_board(b);
		} else {
			printf ("open failed.\n");
		}
		g_free (filename);
	}

	gtk_widget_destroy (dialog);
}

void board_save_dialog (board *b, int save_as)
{
	GtkWidget *dialog;

	if (!save_as && b->filename) {
		board_save(b, b->filename->str);
		return;
	}

	dialog = gtk_file_chooser_dialog_new ("Save File",
			b->win,
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

	if (!b->filename) {
		//gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "hand.lin");
	}
	else
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), b->filename->str);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		if (b->filename)
			g_string_free(b->filename, TRUE);
		b->filename = g_string_new(filename);
		g_free (filename);
		board_save (b, b->filename->str);
	}

	gtk_widget_destroy (dialog);
}
