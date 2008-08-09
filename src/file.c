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

#define _GNU_SOURCE

#include <math.h>
#include <ctype.h>
#include <errno.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "functions.h"
#include "window_card.h"
#include "file.h"

static void
add_filters (GtkFileChooser *dialog)
{
	GtkFileFilter *filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter, _("Bridge files"));
	gtk_file_filter_add_pattern (filter, "*.lin");
	gtk_file_filter_add_pattern (filter, "*.pbn");
	gtk_file_chooser_add_filter (dialog, filter);
	filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter, _("LIN files"));
	gtk_file_filter_add_pattern (filter, "*.lin");
	gtk_file_chooser_add_filter (dialog, filter);
	filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter, _("PBN files"));
	gtk_file_filter_add_pattern (filter, "*.pbn");
	gtk_file_chooser_add_filter (dialog, filter);
	filter = gtk_file_filter_new ();
	gtk_file_filter_set_name (filter, _("All files"));
	gtk_file_filter_add_pattern (filter, "*");
	gtk_file_chooser_add_filter (dialog, filter);
}

/*
 * loading
 */

static char *sane_strtok_r (char *const str, const char *delim, char **saveptr)
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
static int
board_parse_lin (window_board_t *win, char *line, FILE *f)
{
	char *saveptr;
	char *tok;
	int card_nr = 0;

	setlocale (LC_NUMERIC, "C");

	board *b = board_new ();
	int board_filled = 0;
	board_window_append_board (win, b);

	/* global list of names for vugraph files */
	char *name_arr[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	int name_n = 0;
	/* IMP/MP list */
	char *mp_str = NULL;
	char *mp_ptr = NULL;

	do {
	for (tok = sane_strtok_r(line, "|", &saveptr); tok; tok = STRTOK) {
		/* single hand */
		if (!strcmp(tok, "pn")) { /* SWNE */
			tok = STRTOK;
			char *nameptr;
			char *name = sane_strtok_r(tok, ",", &nameptr);
			int i = 0;
			do {
				if (i < 4)
					g_string_printf(b->hand_name[seat_mod (i++) - 1], "%s", name);
				if (name_n < 8)
					name_arr[name_n++] = strdup(name);
			} while ((name = sane_strtok_r(NULL, ",", &nameptr)));

		} else if (!strcmp(tok, "md")) { /* make deal */
			tok = STRTOK;
			if (*tok == '0') {
				printf ("md|0| (keep deal) mode not supported\n");
				continue;
			}
			if (board_filled) {
				/* start new board */
				card_nr = 0;
				board_filled = 0;

				b = board_new ();
				board_window_append_board (win, b);
				/* initialize player names, required for vugraph files */
				int i;
				for (i = 0; i < 4; i++) {
					g_string_printf(b->hand_name[seat_mod (i) - 1],
						"%s", name_arr[i + (tok[0] == 'c' ? 4 : 0)]);
				}
			}
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
						goto error;
				} else if (*c == ',') {
					se = seat_mod(se + 1);
				} else {
					printf("Parse error: %s", tok);
					goto error;
				}
			}
			// TODO: end positions
			deal_random(b); /* compute east hand */
			board_filled = 1; /* consider this board finished on next qx token */

		} else if (!strcmp(tok, "ah")) { /* board name */
			g_string_printf(b->name, "%s", STRTOK);

		} else if (!strcmp(tok, "qx")) { /* board number, o1, c1, o2, ... */
			tok = STRTOK;
			if (board_filled) { /* first token in new vugraph board */
				card_nr = 0;
				board_filled = 0;

				b = board_new ();
				board_window_append_board (win, b);
				/* initialize player names, required for vugraph files */
				int i;
				for (i = 0; i < 4; i++) {
					g_string_printf(b->hand_name[seat_mod (i) - 1],
						"%s", name_arr[i + (tok[0] == 'c' ? 4 : 0)]);
				}
			}
			if (strlen (tok) >= 1)
				g_string_printf(b->name, "%s %s",
					tok[0] == 'c' ? _("Closed") :
						(tok[0] == 'o' ? _("Open") : _("Board")),
					tok + 1);

			/* for now assume qx|| is present in all lin files with mp|| */
			if (mp_str) {
				// FIXME: skip leading boards that were kibitzed but not played
				// (01-26-08-3.lin)
				char *score = mp_ptr ? sane_strtok_r (NULL, ",", &mp_ptr) :
						sane_strtok_r (mp_str, ",", &mp_ptr);
				b->mp[0] = score ? round (strtod (score, NULL) * 100.0) : 0;
				score = sane_strtok_r (NULL, ",", &mp_ptr);
				b->mp[1] = score ? round (strtod (score, NULL) * 100.0) : 0;
			}

		} else if (!strcmp(tok, "sv")) {
			tok = STRTOK;
			switch (*tok) {
				case '0':
				case 'o': b->vuln[0] = 0; b->vuln[1] = 0; break;
				case 'n': b->vuln[0] = 1; b->vuln[1] = 0; break;
				case 'e': b->vuln[0] = 0; b->vuln[1] = 1; break;
				case 'b': b->vuln[0] = 1; b->vuln[1] = 1; break;
				default: printf("Unknown vulnerability: sv|%s|\n", tok);
			}

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
				board_append_bid(b, bid, 1);
				if (al) {
					board_set_alert (b, al);
					al = NULL;
				}
			} while (*bidp);

		} else if (!strcmp(tok, "an")) {
			tok = STRTOK;
			board_set_alert (b, !strcmp (tok, "!") ? "" : tok); /* filter uninteresting ! */

		} else if (!strcmp(tok, "pc")) {
			int c = parse_card(tok = STRTOK);
			if (c == -1) {
				printf("Invalid card %s\n", tok);
				continue;
			}
			if (card_nr < 52)
				b->played_cards[card_nr++] = c;

		} else if (!strcmp(tok, "mc")) {
			tok = STRTOK; // TODO: store number of (total) claimed tricks
			b->played_cards[card_nr] = claim_rest; // no card_nr increment here
			b->declarer_tricks = atoi (tok);

		/* vugraph file */
		} else if (!strcmp(tok, "vg")) { /* match title */
			tok = STRTOK;
			//printf ("Match title: %s\n", tok);
			if (win->title)
				free (win->title);
			if (win->subtitle)
				free (win->subtitle);
			if (win->team1)
				free (win->team1);
			if (win->team2)
				free (win->team2);

			char *t_ptr;
			char *title = sane_strtok_r (tok, ",", &t_ptr);
			if (title)
				win->title = strdup (title);
			char *subtitle = sane_strtok_r (NULL, ",", &t_ptr);
			if (subtitle)
				win->subtitle = strdup (subtitle);
			sane_strtok_r (NULL, ",", &t_ptr); /* scoring I IMPs P MPs B board-a-match */
			sane_strtok_r (NULL, ",", &t_ptr); /* first board nr */
			sane_strtok_r (NULL, ",", &t_ptr); /* last board nr */
			char *team1 = sane_strtok_r (NULL, ",", &t_ptr);
			if (team1)
				win->team1 = strdup (team1);
			sane_strtok_r (NULL, ",", &t_ptr); /* carry-over score team 1 */
			char *team2 = sane_strtok_r (NULL, ",", &t_ptr);
			if (team2)
				win->team2 = strdup (team2);
			/* carry-over score team 2 */

		} else if (!strcmp(tok, "pw")) { /* more player names */
			tok = STRTOK;
			//printf ("Players: %s\n", tok);
		} else if (!strcmp(tok, "bn")) { /* board numbers */
			tok = STRTOK;
			//printf ("Board numbers: %s\n", tok);
		} else if (!strcmp(tok, "rs")) { /* results */
			tok = STRTOK;
			//printf ("Results: %s\n", tok);
		} else if (!strcmp(tok, "mp")) { /* MP result */
			tok = STRTOK;
			//printf ("Scores: %s\n", tok);
			mp_str = strdup (tok);
		} else if (!strcmp(tok, "nt")) { /* comment (new text) */
			tok = STRTOK;
			//printf ("Comment: %s\n", tok);

		} else if (!strcmp(tok, "at")) { /* add text */
			STRTOK;
		} else if (!strcmp(tok, "cr") || !strcmp(tok, "cg") ||
			   !strcmp(tok, "cb")) { /* color */
			STRTOK;
		} else if (!strcmp(tok, "hc") || !strcmp(tok, "lc") ||
			   !strcmp(tok, "hs") || !strcmp(tok, "ls")) {
			STRTOK; /* hilight card, suit */
		} else if (!strcmp(tok, "pg")) {
			STRTOK; /* page break, e.g. after trick or comment */
		} else if (!strcmp(tok, "rh")) { /* reset heading */
			STRTOK;
		} else if (!strcmp(tok, "sk")) { /* set kibitzed */
			STRTOK;
		} else if (!strcmp(tok, "st")) { /* small text */
			STRTOK;
		} else if (!strcmp(tok, "up")) { /* undo play */
			tok = STRTOK;
		} else if (!*tok || *tok == '\n' || *tok == '\r') {
			/* empty token, hopefully end of line */
		} else {
			printf("Unknown token '%s|%s|'\n", tok, STRTOK);
		}
	}
	} while (fgets(line, 1023, f));

	int ret = 1;
	int i;
	goto ok;
error:
	ret = 0;
ok:
	for (i = 0; i < name_n; i++)
		free (name_arr[i]);
	if (mp_str)
		free (mp_str);

	setlocale (LC_NUMERIC, "");

	return ret;
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
	if (!strncmp(buf, "pn|", 3) || !strncmp(buf, "vg|", 3) ||
	    !strncmp(buf, "st|", 3)) {
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
			free (win->filename);
		win->filename = strdup (fname);
	}
	errno = e;
	return ret;
}

#define TRY_FREE(p) if (p) free (p)
#define MOVE_PTR(dst, src) TRY_FREE (dst); dst = src; src = NULL

/* tries opening a file in a temporary win structure, and displays errors if unable */
int
board_load_popup (window_board_t *win, int append, char *filename)
{
	int i;
	int ret = 0;
	window_board_t *win1 = calloc (1, sizeof (window_board_t));

	if (board_load (win1, filename)) {
		if (append) {
			int n = win->n_boards;
			for (i = 0; i < win1->n_boards; i++)
				board_window_append_board (win, win1->boards[i]);
			if (win->n_boards > n) /* set to first new board */
				win->cur = n;
		} else {
			MOVE_PTR (win->filename, win1->filename);
			MOVE_PTR (win->title, win1->title);
			MOVE_PTR (win->subtitle, win1->subtitle);
			MOVE_PTR (win->team1, win1->team1);
			MOVE_PTR (win->team2, win1->team2);

			for (i = 0; i < win->n_boards; i++)
				if (win->boards[i])
					board_free (win->boards[i]);
			win->boards = win1->boards;
			win->n_boards = win1->n_boards;
			win->n_boards_alloc = win1->n_boards_alloc;
			win->cur = 0;
		}

		card_window_update(win->boards[win->cur]->dealt_cards);
		show_board(win->boards[win->cur], REDRAW_FULL);
		recently_used_add (filename);

		ret = 1;
	} else {
		GtkWidget *error = gtk_message_dialog_new (GTK_WINDOW (win->window),
				GTK_DIALOG_DESTROY_WITH_PARENT,
				GTK_MESSAGE_ERROR,
				GTK_BUTTONS_CLOSE,
				_("Error loading file '%s': %s"),
				filename, g_strerror (errno));
		gtk_dialog_run (GTK_DIALOG (error));
		gtk_widget_destroy (error);
	}

	TRY_FREE (win1->filename);
	TRY_FREE (win1->title);
	TRY_FREE (win1->subtitle);
	TRY_FREE (win1->team1);
	TRY_FREE (win1->team2);
	free (win1);

	return ret;
}

int
board_load_dialog (window_board_t *win, int append)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new (_("Open File"),
			GTK_WINDOW (win->window),
			GTK_FILE_CHOOSER_ACTION_OPEN,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
			NULL);
	add_filters (GTK_FILE_CHOOSER (dialog));

	if (win->filename) {
		char *cwd = g_path_get_dirname (win->filename);
		gtk_file_chooser_set_current_folder (GTK_FILE_CHOOSER (dialog), cwd);
		g_free (cwd);
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), win->filename);
	}

	int ret = 0;
	while (ret == 0) {
		if (gtk_dialog_run (GTK_DIALOG (dialog)) != GTK_RESPONSE_ACCEPT)
			break;
		char *filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		ret = board_load_popup (win, append, filename);
		g_free (filename);
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
	snprintf(buf, sizeof (buf), "%d%c", LEVEL(bid), "CDHSN"[DENOM(bid)]);
	return buf;
}

static char *
lin_contract (board *b)
{
	static char buf[10];
	if (b->level == 0) {
		snprintf (buf, sizeof (buf), "P");
	} else {
		snprintf (buf, sizeof (buf), "%d%s%s%s%s",
			b->level, trump_str_char[b->trumps],
			seat_str[b->declarer], double_str[b->doubled],
			overtricks (b->declarer_tricks - b->level - 6));
	}
	return buf;
}

static int
board_save_lin(window_board_t *win, char *filename)
{
	int cur;
	FILE *f;
	if ((f = fopen (filename, "w")) == NULL)
		return 0;

	setlocale (LC_NUMERIC, "C");

	if (win->title) {
		fprintf (f, "vg|%s,%s,%s,%d,%d,%s,,%s,|\n",
			win->title,
			win->subtitle ? win->subtitle : "",
			"P", // FIXME
			1, win->n_boards,
			win->team1 ? win->team1 : "",
			win->team2 ? win->team2 : "");

		fprintf (f, "rs|");
		for (cur = 0; cur < win->n_boards; cur++) {
			board *b = win->boards[cur];
			fprintf (f, "%s,", lin_contract (b));
			if (cur != win->n_boards - 1)
				fprintf (f, ",");
		}
		fprintf (f, "|\n");

		fprintf (f, "pw|");
		for (cur = 0; cur < win->n_boards; cur++) {
			board *b = win->boards[cur];
			fprintf (f, "%s,%s,%s,%s",
				b->hand_name[south-1]->str, b->hand_name[west-1]->str,
				b->hand_name[north-1]->str, b->hand_name[east-1]->str);
			if (cur != win->n_boards - 1)
				fprintf (f, ",");
		}
		fprintf (f, "|\n");

		fprintf (f, "mp|");
		for (cur = 0; cur < win->n_boards; cur++) {
			board *b = win->boards[cur];
			if (b->mp[0] == 0 && b->mp[1] == 0)
				fprintf (f, "--,--");
			else {
				if (b->mp[0])
					fprintf (f, "%.2f", b->mp[0] / 100.0);
				fprintf (f, ",");
				if (b->mp[1])
					fprintf (f, "%.2f", b->mp[1] / 100.0);
			}
			if (cur != win->n_boards - 1)
				fprintf (f, ",");
		}
		fprintf (f, "|\n");

		fprintf (f, "bn|");
		for (cur = 0; cur < win->n_boards; cur++) {
			//board *b = win->boards[cur];
			fprintf (f, "%d", cur + 1); // TODO: original number?
			if (cur != win->n_boards - 1)
				fprintf (f, ",");
		}
		fprintf (f, "|\n");

		fprintf (f, "pg||\n");
	}

	for (cur = 0; cur < win->n_boards; cur++) {
		board *b = win->boards[cur];
		int i;

		if (win->n_boards > 1)
			fprintf (f, "qx|o%d|", cur + 1); // TODO: open/closed, real board number

		fprintf (f, "pn|%s,%s,%s,%s|",
			b->hand_name[south-1]->str, b->hand_name[west-1]->str,
			b->hand_name[north-1]->str, b->hand_name[east-1]->str);
		fprintf (f, "st||");
		fprintf (f, "md|%d%s|", seat_mod(b->dealer + 1), lin_card_string(b)); // TODO: end positions
		fprintf (f, "rh||");
		fprintf (f, "ah|%s|", b->name->str);
		fprintf (f, "sv|%c|", b->vuln[0] ? (b->vuln[1] ? 'b' : 'n')
						: (b->vuln[1] ? 'e' : 'o'));
		for (i = 0; i < b->n_bids; i++) {
			fprintf(f, "mb|%s|", lin_bid(b->bidding[i]));
			if (b->alerts[i])
				fprintf (f, "an|%s|", *b->alerts[i] ? b->alerts[i] : "!");
		}
		for (i = 0; i < 52; i++) {
			if (i % 4 == 0)
				fprintf(f, "pg||");
			card c = b->played_cards[i];
			if (c < 0)
				break;
			if (c == claim_rest) {
				fprintf(f, "mc|%d|",
					b->declarer_tricks >= 0 ? b->declarer_tricks : 0);
				break;
			}
			fprintf(f, "pc|%c%c|", "CDHS"[SUIT(c)], rank_char(RANK(c)));
		}
		fprintf (f, "pg||\n");
	}

	int ret = 1, e = 0;
	if (ferror (f)) {
		ret = 0;
		e = errno;
	}
	fclose(f);
	errno = e;

	setlocale (LC_NUMERIC, "");

	return ret;
}

int
board_save(window_board_t *win, char *filename)
{
	int len = strlen(filename);
	char *suffix = len > 4 ? filename + len - 4 : filename;
	int ret = 0;
	if (!strcmp(suffix, ".lin")) {
		ret = board_save_lin (win, filename);
	} else if (!strcmp(suffix, ".pbn")) {
		printf("pbn not yet implemented\n");
		errno = EMEDIUMTYPE;
	} else {
		printf("unrecognized suffix %s\n", suffix);
		errno = EMEDIUMTYPE;
	}
	return ret;
}

int
board_save_dialog (window_board_t *win, int save_as)
{
	GtkWidget *dialog;

	if (!save_as && win->filename) {
		int ret = board_save(win, win->filename);
		if (! ret) {
			GtkWidget *error = gtk_message_dialog_new (GTK_WINDOW (win->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					_("Error saving file '%s': %s"),
					win->filename, g_strerror (errno));
			gtk_dialog_run (GTK_DIALOG (error));
			gtk_widget_destroy (error);
		}
		return ret;
	}

	dialog = gtk_file_chooser_dialog_new (_("Save File"),
			GTK_WINDOW (win->window),
			GTK_FILE_CHOOSER_ACTION_SAVE,
			GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
			GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
			NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
	add_filters (GTK_FILE_CHOOSER (dialog));

	if (!win->filename)
		gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), _("hand.lin"));
	else
		gtk_file_chooser_set_filename (GTK_FILE_CHOOSER (dialog), win->filename);

retry_save:
	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;

		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		int ret = board_save (win, filename);

		if (ret) {
			if (win->filename)
				free (win->filename);
			win->filename = filename;
			show_board (CUR_BOARD, REDRAW_TITLE);
			recently_used_add (filename);
		} else {
			GtkWidget *error = gtk_message_dialog_new (GTK_WINDOW (win->window),
					GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE,
					_("Error saving file '%s': %s"),
					filename, g_strerror (errno));
			g_free (filename);
			gtk_dialog_run (GTK_DIALOG (error));
			gtk_widget_destroy (error);
			goto retry_save;
		}
	}

	gtk_widget_destroy (dialog);
	return 1;
}
