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
#include <dds.h>
#include <stdlib.h> /* system */

#include "support.h" /* lookup_widget */
#include "bridge.h"
#include "file.h" /* board_format_line */
#include "functions.h"
#include "window_board.h" /* statusbar */

void init_solve()
{
	InitStart();
}

static const char *dds_error[] = {
          NULL,
 /* -1 */ "Unknown fault",
 /* -2 */ "No of cards = 0",
 /* -3 */ "target > Number of tricks left",
 /* -4 */ "Duplicated cards",
 /* -5 */ "target < -1",
          NULL,
 /* -7 */ "target >13",
 /* -8 */ "solutions < 1",
 /* -9 */ "solutions > 3",
/* -10 */ "No of cards > 52",
/* -11 */ "For mode=2: target =-1 or solutions != 2",
/* -12 */ "Suit or rank value out of range for deal.currentTrickSuit or deal.currentTrickRank",
	  NULL,
};

static const int card_bits[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000};

int run_dd = 0;

void solve_statusbar(char *text)
{
	static guint id = 0;
	if (!id)
		id = gtk_statusbar_get_context_id(win->statusbar, "solve_c");

	gtk_statusbar_pop(win->statusbar, id);
	if (text)
		gtk_statusbar_push(win->statusbar, id, text);
}

static int dds_suit_conv(int s) /* works both ways */
{
	return s == NT ? NT : 3 - s;
}

/*
void board_dds(board *b)
{
	FILE *f;
	int i;
	char tr[] = { 'c', 'd', 'h', 's', 'n'};
	char le[] = { 0, 'w', 'n', 'e', 's' };

	if (!(f = fopen("dd", "w"))) {
		perror("dd");
		return;
	}

	card lead = b->played_cards[b->n_played_cards - (b->n_played_cards % 4)];
	seat leader = b->n_played_cards % 4 == 0 ? b->current_turn : b->dealt_cards[lead];
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

	GString *out = board_format_line(b, ' ', '.');
	fprintf(f, "%s\n", out->str);
	fclose(f);
	g_string_free(out, TRUE);
	system("dds dd&");
}
*/

static int score_to_tricks(board *b, int score) /* result: tricks for declarer */
{
	score += b->tricks[b->current_turn % 2];
	if ((b->current_turn % 2) != (b->declarer % 2))
		score = 13 - score;
	return score;
}

static int
compute_dd_scores0 (board *b, dd_t *dd, card next)
{
	int i, j, c;
	card old_next;
	struct deal d;
	struct futureTricks fut;
	char str[100];

	if (!assert_board(b)) { /* FIXME: do not call this every time */
		board_statusbar(_("Error: hands have different numbers of cards"));
		return 0;
	}

	if (next >= 0) { /* what-if mode: temporarily play this card */
		old_next = b->played_cards[b->n_played_cards];
		b->played_cards[b->n_played_cards] = c;
		b->n_played_cards++;
	}

	/* 0-3, 0=North, 1=East, 2=South, 3=West , Leading hand for the trick.*/
	card lead = b->played_cards[b->n_played_cards - (b->n_played_cards % 4)];
	seat leader = b->n_played_cards % 4 == 0 ? b->current_turn : b->dealt_cards[lead];
	d.first = (leader + 2) % 4;
	d.trump = dds_suit_conv(b->trumps);
	for (i = 0; i < 4; i++) {
		d.currentTrickSuit[i] = 0;
		d.currentTrickRank[i] = 0;
		for (j = 0; j < 4; j++)
			d.remainCards[i][j] = 0;
	}

	for (c = 0; c < 52; c++) {
		if (b->cards[c]) {
			d.remainCards[(b->cards[c] + 2) % 4][3 - SUIT(c)] |= card_bits[RANK(c)];
		}
		dd->card_score[c] = -1;
	}
	for (i = 0; i < b->n_played_cards % 4; i++) {
		card c = b->played_cards[b->n_played_cards - (b->n_played_cards % 4) + i];
		d.currentTrickSuit[i] = 3 - SUIT(c);
		d.currentTrickRank[i] = RANK(c) + 2;
	}

	if (next >= 0) { /* undo what-if change */
		b->n_played_cards--;
		b->played_cards[b->n_played_cards] = old_next;
	}

	solve_statusbar(_("Thinking..."));
	while (gtk_events_pending ())
		gtk_main_iteration();
	i = SolveBoard(d, -1, 3, 1, &fut);
	solve_statusbar(NULL);
	if (i <= 0) {
		snprintf(str, 99, "DD Error: %s", dds_error[-i]);
		board_statusbar(str);
		return 0;
	}
	//printf("solve nodes: %d cards: %d\n", fut.nodes, fut.cards);

	for (i = 0; i < fut.cards; i++) {
		c = 13 * (3 - fut.suit[i]) + fut.rank[i] - 2;
		//printf("card: %s = %d\n", card_string(c)->str, fut.score[i]);
		dd->card_score[c] = score_to_tricks(b, fut.score[i]);

		for (j = fut.rank[i] - 2; j >= 0; j--) { /* equals */
			if (fut.equals[i] & card_bits[j]) {
				c = 13 * (3 - fut.suit[i]) + j;
				//printf("      %s = %d\n", card_string(c)->str, fut.score[i]);
				dd->card_score[c] = score_to_tricks(b, fut.score[i]);
			}
		}
	}

	dd->best_score = score_to_tricks(b, fut.score[0]);
	return 1;
}

/* (optionally!) compute DD scores per card and display optimal DD score in statusbar */
void
compute_dd_scores (board *b, int compute)
{
	if (b->n_played_cards == 52)
		return;

	if (compute && !b->current_dd) {
		b->current_dd = malloc (sizeof (dd_t));
		assert (b->current_dd);
		if (!compute_dd_scores0(b, b->current_dd, -1))
			return;
	}

	/* next card not defined (i.e. no prior undo), set it to the highest
	 * optimal card */ // FIXME: 1st and 2nd hand low
	if (b->current_dd && seat_mask (b->current_turn, win->show_dd_scores) &&
			b->played_cards[b->n_played_cards] == -1) { // FIXME: update when different card is played?
		int c;
		for (c = 51; c >= 0; c--) {
			if (b->current_dd->card_score[c] == b->current_dd->best_score) {
				b->played_cards[b->n_played_cards] = c;
				break;
			}
		}
	}

	if (b->current_dd)
		board_statusbar (score_string(b->level, b->trumps, b->declarer,
				b->doubled, b->vuln[b->declarer % 2],
				b->current_dd->best_score, b->current_turn));
}

/* // FIXME: reanimate
void compute_next_dd_scores(board *b, card c)
{
	if (!b->next_dd[c]) {
		b->next_dd[c] = malloc (sizeof (dd_t));
		assert (b->next_dd[c]);
		compute_dd_scores0(b, b->next_dd[c], c);
	}
}
*/

/*
 * Parscore computation
 */

static int
compute_par_arr(board *b)
{
	int i, j, c;
	struct deal d;
	struct futureTricks fut;

	for (i = 0; i < 4; i++) {
		d.currentTrickSuit[i] = 0;
		d.currentTrickRank[i] = 0;
		for (j = 0; j < 4; j++)
			d.remainCards[i][j] = 0;
	}

	for (c = 0; c < 52; c++) {
		if (b->dealt_cards[c]) {
			d.remainCards[(b->dealt_cards[c] + 2) % 4][3 - SUIT(c)] |= card_bits[RANK(c)];
		}
	}

	GString *str = g_string_new(_("Thinking..."));
	int h, t;
	for (t = 4; t >= 0; t--) {
		g_string_append_printf(str, " %s ", _(trump_str[t]));

		for (h = 0; h < 4; h++) {
			g_string_append_printf(str, "%s", _(seat_str[h + 1]));
			solve_statusbar(str->str);
			while (gtk_events_pending ())
				gtk_main_iteration();

			d.trump = dds_suit_conv(t);
			d.first = h;
			i = SolveBoard(d, -1 /* target */, 1 /* solutions */,
					h == 0 ? 1 : 2 /* mode */, &fut);
			if (i <= 0) {
				g_string_printf(str, "DD Error: %s", dds_error[-i]);
				solve_statusbar(NULL);
				board_statusbar(str->str);
				g_string_free(str, TRUE);
				return 0;
			}
			b->par_arr[h][t] = 13 - fut.score[0];
			/* we could store (one) optimum lead here */
			//printf("t %d; h %d = %d\n", t, h, 13 - fut.score[0]);
		}
	}

	solve_statusbar(NULL);
	g_string_free(str, TRUE);
	//system("dds -tricks dd&");
	return 1;
}

void parscore(board *b)
{
	int l, t;

	if (b->par_score == -1) {
		if (b->n_dealt_cards != 52) {
			board_statusbar (_("Error: board must be fully dealt"));
			return;
		}
		if (!compute_par_arr(b))
			return;
	}

	b->par_score = 0;

	for (l = 1; l <= 7; l++) {
		for (t = club; t <= NT; t++) {
			int ha = north, tr = b->par_arr[1][t];
			if (b->par_arr[3][t] > b->par_arr[1][t]) {
				ha = south;
				tr = b->par_arr[3][t];
			}
			int sc = score(l, t, -1, b->vuln[0], tr);
			//printf("%s %d\n", contract_string(l, t, north, 0), sc);
			if (sc > b->par_score) {
				b->par_score = sc;
				b->par_level = l;
				b->par_suit = t;
				b->par_dec = ha;
				b->par_tricks = tr;
				//printf("  new par\n");
			}
			ha = east, tr = b->par_arr[2][t];
			if (b->par_arr[0][t] > b->par_arr[2][t]) {
				ha = west;
				tr = b->par_arr[0][t];
			}
			sc = -score(l, t, -1, b->vuln[1], tr);
			//printf("%s %d\n", contract_string(l, t, east, 0), sc);
			if (sc < b->par_score) {
				b->par_score = sc;
				b->par_level = l;
				b->par_suit = t;
				b->par_dec = ha;
				b->par_tricks = tr;
				//printf("  new par\n");
			}
		}
	}
}

char *
par_label (board *b)
{
	GString *par = g_string_new(_("Par: PASS (0)\n"));
	if (b->par_score != 0)
		g_string_printf(par, _("Par: %s %s (%+d)\n"),
			contract_string(b->par_level, b->par_suit, b->par_dec,
				b->par_tricks < b->par_level + 6),
			overtricks(b->par_tricks - b->par_level - 6),
			b->par_score);

	int t;
	for (t = 4; t >= 0; t--) {
		g_string_append_printf(par, "%s: ", _(trump_str[t]));
		if (b->par_arr[1][t] == b->par_arr[3][t])
			g_string_append_printf(par, "%s<b>%d</b> ", _("NS"), b->par_arr[1][t]);
		else
			g_string_append_printf(par, "%s<b>%d</b>%s<b>%d</b> ",
					_("N"), b->par_arr[1][t], _("S"), b->par_arr[3][t]);
		if (b->par_arr[0][t] == b->par_arr[2][t])
			g_string_append_printf(par, "%s<b>%d</b>", _("EW"), b->par_arr[2][t]);
		else
			g_string_append_printf(par, "%s<b>%d</b>%s<b>%d</b>",
					_("E"), b->par_arr[2][t], _("W"), b->par_arr[0][t]);
		if (t > 0)
			g_string_append_printf(par, "\n");
	}

	char *ret = strdup (par->str);
	g_string_free(par, TRUE);
	return ret;
}
