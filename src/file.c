/*
 *  tenace - bridge hand viewer and editor
 *  Copyright (C) 2005-2007 Christoph Berg <cb@df7cb.de>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "functions.h"
#include "window_card.h"
#include "file.h"

/*
 * loading
 */

static char *sane_strtok_r (char *str, const char *delim, char **saveptr)
{
	char *this = str ? str : *saveptr;
	if (!this)
		return NULL;

	char *next = strpbrk (this, delim);
	if (next) {
		*next = '\0';
		*saveptr = next + 1;
	} else {
		*saveptr = NULL;
	}
	return this;
}

// pn|Frederic,gm,Myon,mecky|st||md|4S27KAHKD49C3589TJ,S36H48AD358KC27QK,STJH259TJQD26TJQC,|rh||ah|Board 14|sv|o|mb|p|mb|p|mb|1D|mb|2H|mb|2S|mb|p|mb|p|mb|p|pg||pc|HK|pc|H4|pc|H2|pc|H6|pg||pc|CJ|pc|CK|pc|ST|pc|C4|pg||pc|H9|pc|H3|pc|S2|pc|H8|pg||pc|CT|pc|C2|pc|SJ|pc|C6|pg||pc|HQ|pc|H7|pc|S7|pc|HA|pg||pc|D9|pc|D3|pc|DT|pc|DA|pg||pc|S4|pc|SK|pc|S3|pc|H5|pg||pc|SA|pc|S6|pc|HT|pc|S5|pg||pc|D4|pc|DK|pc|D2|pc|D7|pg||pc|C7|pc|D6|pc|CA|pc|C3|pg||mc|6|
#define STRTOK sane_strtok_r(NULL, "|\n\r", &saveptr)
#define FINISH_BOARD \
	/*printf ("finish: %d\n", contract);*/ \
	if (contract && b->played_cards[0] != -1) \
		board_set_contract(b, LEVEL(contract), DENOM(contract), \
			seat_mod(b->dealt_cards[b->played_cards[0]] + 3), doubled); \
	card_nr = 0; \
	contract = 0; \
	doubled = 0;
static int
board_parse_lin (window_board_t *win, char *line, FILE *f)
{
	char *saveptr;
	char *tok;
	int card_nr = 0;
	int contract = 0;
	int doubled = 0;
	int i;

	board *b = board_new ();
	int board_filled = 0;
	board_window_append_board (win, b);

	char *name_arr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int name_n = 0;

	do {
	for (tok = sane_strtok_r(line, "|", &saveptr); tok; tok = STRTOK) {
		//printf ("parsing token '%s'\n", tok);
		/* single hand */
		if (!strcmp(tok, "pn")) { /* SWNE */
			tok = STRTOK;
			char *nameptr;
			char *name = sane_strtok_r(tok, ",", &nameptr);
			assert (name_n < 8);
			do {
				name_arr[name_n] = strdup(name);
				if (name_n < 4)
					g_string_printf(b->hand_name[seat_mod (name_n) - 1], "%s", name);
				name_n++;
			} while ((name = sane_strtok_r(NULL, ",", &nameptr)) && name_n < 8);
			//g_string_printf(b->hand_name[south-1], "%s", strtok_r(NULL, ",|", &saveptr));
			//g_string_printf(b->hand_name[west-1], "%s", strtok_r(NULL, ",|", &saveptr));
			//g_string_printf(b->hand_name[north-1], "%s", strtok_r(NULL, ",|", &saveptr));
			//g_string_printf(b->hand_name[east-1], "%s", strtok_r(NULL, ",|", &saveptr));
		} else if (!strcmp(tok, "md")) {
			assert (!board_filled);
			tok = STRTOK;
			char *c;
			seat se = south;
			suit su = spade;
			b->dealer = seat_mod(*tok - '0' - 1);
			for (c = tok + 1; *c; c++) {
				if ((i = parse_suit(*c)) != -1) {
					su = i;
				} else if ((i = parse_rank_char(*c)) != -1) {
					if (add_card(b, se, (su * 13) + i) != 1)
						goto error;
				} else if (*c == ',') {
					se = seat_mod(se + 1);
				} else {
					printf("Parse error: %s", tok);
					goto error;
				}
			}
			deal_random(b); /* compute east hand */
			board_filled = 1; /* consider this board finished on next qx token */
		} else if (!strcmp(tok, "ah")) { /* board name */
			g_string_printf(b->name, "%s", STRTOK);
		} else if (!strcmp(tok, "qx")) { /* board number, o1, c1, o2, ... */
			tok = STRTOK;
			if (board_filled) { /* first token in new vugraph board */
				FINISH_BOARD;
				board_filled = 0;

				/* initialize player names for new board */
				b = board_new ();
				board_window_append_board (win, b);
				int i;
				for (i = 0; i < 4; i++) {
					g_string_printf(b->hand_name[seat_mod (i) - 1],
						"%s", name_arr[i + (tok[0] == 'c' ? 4 : 0)]);
				}
			}
			/*
			if (res) {
				if (res_ptr)
					b->name2 = g_string_new (strtok_r (NULL, ",", &res_ptr));
				else
					b->name2 = g_string_new (strtok_r (res, ",", &res_ptr));
			}
			*/
			if (strlen (tok) >= 1)
				g_string_printf(b->name, "%s %s",
					tok[0] == 'c' ? "Closed" : "Open", tok + 1);
		} else if (!strcmp(tok, "sv")) {
			tok = STRTOK;
			switch (*tok) {
				case 'o': b->vuln[0] = 0; b->vuln[1] = 0; break;
				case 'n': b->vuln[0] = 1; b->vuln[1] = 0; break;
				case 'e': b->vuln[0] = 1; b->vuln[1] = 1; break;
				case 'b': b->vuln[0] = 1; b->vuln[1] = 1; break;
				default: printf("Unknown vulnerability: %s\n", tok);
			}
		} else if (!strcmp(tok, "pw")) { /* more player names */
			tok = STRTOK;
		} else if (!strcmp(tok, "mb")) {
			/* mb|-ppp1Cp1Hp3Np4Dp4Hppp| */
			tok = STRTOK;
			char *bidp = tok;
			char *al = strchr (bidp, '!');
			if (al) {
				*al++ = '\0';
			}
			do {
				int bid = parse_bid(&bidp);
				if (bid == -1) {
					printf("Invalid bid %s/%s\n", tok, bidp);
					break;
				}
				board_append_bid(b, bid);
				if (al) {
					board_set_alert (b, al);
					al = NULL;
				}
				if (bid > bid_xx) {
					contract = bid;
					doubled = 0;
				} else if (bid == bid_x || bid == bid_xx) {
					doubled = bid;
				}
			} while (*bidp);
		} else if (!strcmp(tok, "an")) {
			tok = STRTOK;
			board_set_alert (b, !strcmp (tok, "!") ? "" : tok); /* filter uninteresting ! */
		} else if (!strcmp(tok, "pc")) {
			int c = parse_card(tok = STRTOK);
			if (c == -1) {
				printf("Invalid card %s\n", tok);
				goto error;
			}
			if (card_nr < 52)
				b->played_cards[card_nr++] = c;
		} else if (!strcmp(tok, "st")) {
			STRTOK;
		} else if (!strcmp(tok, "rh")) {
			STRTOK;
		} else if (!strcmp(tok, "pg")) { /* new page, e.g. after trick or comment */
			STRTOK;
		} else if (!strcmp(tok, "mc")) {
			b->played_cards[card_nr] = claim_rest; // no card_nr increment here
			STRTOK;
		/* vugraph file */
		} else if (!strcmp(tok, "vg")) { /* match title */
			tok = STRTOK;
			printf ("Match title: %s\n", tok);
			char *t_ptr;
			char *title = sane_strtok_r (tok, ",", &t_ptr);
			char *subtitle = sane_strtok_r (NULL, ",", &t_ptr);
			sane_strtok_r (NULL, ",", &t_ptr);
			sane_strtok_r (NULL, ",", &t_ptr);
			sane_strtok_r (NULL, ",", &t_ptr);
			char *team1 = sane_strtok_r (NULL, ",", &t_ptr);
			sane_strtok_r (NULL, ",", &t_ptr);
			char *team2 = sane_strtok_r (NULL, ",", &t_ptr);
			if (win->title)
				g_string_free (win->title, TRUE);
			win->title = g_string_new (NULL);
			g_string_printf (win->title, "%s %s %s - %s", title, subtitle, team1, team2);
		} else if (!strcmp(tok, "rs")) { /* results */
			STRTOK;
		} else if (!strcmp(tok, "mp")) { /* MP result */
			STRTOK;
		} else if (!strcmp(tok, "nt")) { /* comment */
			tok = STRTOK;
			//printf ("\"%s\"\n", tok);
		} else if (!*tok) {
			// empty token, hopefully end of line
		} else {
			printf("Unknown token '%s'\n", tok);
		}
	}
	} while (fgets(line, 1023, f));

	FINISH_BOARD;
	int ret = 1;
	goto ok;
error:
	ret = 0;
ok:
	for (i = 0; i < name_n; i++)
		free (name_arr[i]);
	//printf ("returning %d\n", ret);
	return ret;
}
#undef STRTOK
#undef FINISH_BOARD

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

int
board_load (window_board_t *win, char *fname)
{
	FILE *f;
	char buf[1024];
	if (! (f = fopen (fname, "r")))
		return 0;
	if (fgets (buf, 1023, f) == NULL)
		return 0;

	int ret;
	if (!strncmp(buf, "pn", 2) || !strncmp(buf, "vg", 2)) {
		ret = board_parse_lin (win, buf, f);
	} else {
		board *b = board_new ();
		ret = board_parse_line(buf, b, ' ', '.');
		if (ret)
			board_window_append_board (win, b);
		else {
			errno = EMEDIUMTYPE;
			board_free (b);
		}
	}
	int e = errno;
	fclose(f);
	if (ret) {
		if (win->filename)
			g_string_free (win->filename, TRUE);
		win->filename = g_string_new (fname);
	}
	errno = e;
	return ret;
}

int
board_load_dialog (window_board_t *win, int append)
{
	GtkWidget *dialog;
	int ret = 0;
	int i;

	dialog = gtk_file_chooser_dialog_new ("Open File",
			GTK_WINDOW (win->window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		window_board_t *win1 = malloc(sizeof(window_board_t));
		win1->n_boards = win1->n_boards_alloc = 0;
		win1->filename = win1->title = NULL;

		if (board_load (win1, filename)) {
			if (append) {
				int n = win->n_boards;
				for (i = 0; i < win1->n_boards; i++)
					board_window_append_board (win, win1->boards[i]);
				if (win->n_boards > n) /* set to first new board */
					win->cur = n;
			} else {
				if (win->filename)
					g_string_free (win->filename, TRUE);
				win->filename = win1->filename;
				if (win->title)
					g_string_free (win->title, TRUE);
				win->title = win1->title;
				for (i = 0; i < win->n_boards_alloc; i++)
					if (win->boards[i])
						board_free (win->boards[i]);
				win->boards = win1->boards;
				win->n_boards = win1->n_boards;
				win->n_boards_alloc = win1->n_boards_alloc;
				win->cur = 0;
			}

			card_window_update(win->boards[win->cur]->dealt_cards);
			show_board(win->boards[win->cur], REDRAW_FULL);
			ret = 1;
		} else {
			GtkWidget *error = gtk_message_dialog_new (GTK_WINDOW (win->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					"Error loading file '%s': %s",
					filename, g_strerror (errno));
			gtk_dialog_run (GTK_DIALOG (error));
			gtk_widget_destroy (error);
		}

		free (win1);
	}

	gtk_widget_destroy (dialog);
	return ret;
}

/*
 * saving
 */

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

static int
board_save_lin(window_board_t *win, char *filename)
{
	FILE *f;
	int i;

	if (!(f = fopen(filename, "w")))
		return 0;

	board *b = CUR_BOARD; // FIXME

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

	int ret = 1, e = 0;
	if (fprintf (f, "\n") <= 0) {
		ret = 0;
		e = errno;
	}
	fclose(f);
	errno = e;
	return ret;
}

int
board_save(window_board_t *win, char *filename)
{
	int len, ret;
	if ((len = strlen(filename)) > 4) {
		if (!strcmp(filename + len - 4, ".lin")) {
			ret = board_save_lin (win, filename);
		} else if (!strcmp(filename + len - 4, ".pbn")) {
			printf("pbn not yet implemented\n");
			ret = 0;
			errno = EMEDIUMTYPE;
		} else {
			printf("unrecognized suffix\n");
			ret = 0;
			errno = EMEDIUMTYPE;
			/*
			int h;
			printf("%s\n", b->name->str);
			for (h = 1; h < 5; h++) {
				printf("%s\n", b->hand_name[h-1]->str);
				printf("%s\n", hand_string(b, h)->str);
			}
			*/
		}
	}
	return ret;
}

int
board_save_dialog (window_board_t *win, int save_as)
{
	GtkWidget *dialog;

	if (!save_as && win->filename) {
		board_save(win, win->filename->str);
		return;
	}

	dialog = gtk_file_chooser_dialog_new ("Save File",
			GTK_WINDOW (win->window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

	if (!win->filename) {
		//gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), default_folder_for_saving);
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "hand.lin");
	}
	else
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), win->filename->str);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		board_save (win, filename);

		//XXX
		if (win->filename)
			g_string_free(win->filename, TRUE);
		win->filename = g_string_new(filename);
		g_free (filename);
		show_board (CUR_BOARD, REDRAW_TITLE);
	}

	gtk_widget_destroy (dialog);
	return 1;
}
