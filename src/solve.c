#include <assert.h>
#include <dds.h>
#include "support.h" /* lookup_widget */
#include "board.h"
#include "main.h"

void init_solve()
{
	InitStart();
}

static const char *dds_error[] = {
          "",
 /* -1 */ "Unknown fault",
 /* -2 */ "No of cards = 0",
 /* -3 */ "target > Number of tricks left",
 /* -4 */ "Duplicated cards",
 /* -5 */ "target < -1",
          "",
 /* -7 */ "target >13",
 /* -8 */ "solutions < 1",
 /* -9 */ "solutions > 3",
/* -10 */ "No of cards > 52",
/* -11 */ "For mode=2: target =-1 or solutions != 2",
/* -12 */ "Suit or rank value out of range for deal.currentTrickSuit or deal.currentTrickRank",
};

static const int card_bits[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000};

static guint status_id = 0;

static int dds_suit_conv(int s) /* works both ways */
{
	return s == NT ? NT : 3 - s;
}

void solve_board(board *b)
{
	int i, j, c;
	struct deal d;
	struct futureTricks fut;
	char str[100];

	GtkStatusbar *statusbar;
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	if (!status_id)
		status_id = gtk_statusbar_get_context_id(statusbar, "solve");

	/*
	FIXME: update check for partially played tricks
	for (i = north; i <= south; i++)
		if (b->hands[i-1]->ncards != b->hands[west-1]->ncards) {
			snprintf(str, 99, "Error: %s has %d cards while %s has %d",
				seat_string(west), b->hands[west-1]->ncards,
				seat_string(i), b->hands[i-1]->ncards);
			gtk_statusbar_push(statusbar, status_id, str);
			return;
		}
		*/

	d.trump = dds_suit_conv(b->trumps);
	/* 0-3, 0=North, 1=East, 2=South, 3=West , Leading hand for the trick.*/
	seat leader = b->n_played_cards % 4 == 0 ? b->current_lead :
		b->played_cards_seat[b->n_played_cards - (b->n_played_cards % 4)];
	d.first = (leader + 2) % 4;
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
	}
	for (i = 0; i < b->n_played_cards % 4; i++) {
		card c = b->played_cards[b->n_played_cards - (b->n_played_cards % 4) + i];
		d.currentTrickSuit[i] = 3 - SUIT(c);
		d.currentTrickRank[i] = RANK(c) + 2;
	}

	gtk_statusbar_push(statusbar, status_id, "Thinking...");
	gtk_main_iteration_do(TRUE); // TODO: fixme
	i = SolveBoard(d, -1, 3, 1, &fut);
	if (i <= 0) {
		snprintf(str, 99, "DD Error: %s", dds_error[-i]);
		gtk_statusbar_push(statusbar, status_id, str);
		return;
	}
	gtk_statusbar_pop(statusbar, status_id);
	printf("solve nodes: %d cards: %d\n", fut.nodes, fut.cards);

	label_clear_markups();

	int side = b->current_lead % 2;
	printf("%d has %d tricks, needs %d\n", side, b->tricks[side], b->target[side]);
	for (i = 0; i < fut.cards; i++) {
		c = 13 * (3 - fut.suit[i]) + fut.rank[i] - 2;
		printf("card: %s = %d\n", card_string(c)->str, fut.score[i]);
		b->card_score[c] = fut.score[i];

		char *color = b->tricks[side] + fut.score[i] >= b->target[side] ? "green" : "red";
		char *weight = fut.score[i] == fut.score[0] ? " weight=\"bold\"" : "";
		snprintf(str, 99, "<span background=\"%s\"%s>%s</span>",
			color, weight, rank_string(fut.rank[i]-2));
		label_set_markup(c, str);
		for (j = fut.rank[i] - 2; j >= 0; j--) { /* equals */
			if (fut.equals[i] & card_bits[j]) {
				c = 13 * (3 - fut.suit[i]) + j;
				printf("      %s = %d\n", card_string(c)->str, fut.score[i]);
				b->card_score[c] = fut.score[i];

				snprintf(str, 99, "<span background=\"%s\"%s>%s</span>",
					color, weight, rank_string(j));
				label_set_markup(c, str);
			}
		}
	}

	assert(fut.cards);
	snprintf(str, 99, "DD: %s %s (%d)",
		contract_string(b->level, b->trumps, b->declarer, b->doubled),
		overtricks(7 - b->level - fut.score[0]),
		score(b->level, b->trumps, b->doubled, b->vuln[b->declarer % 2], 13 - fut.score[0]));
	gtk_statusbar_push(statusbar, status_id, str);
}

void parscore(board *b)
{
	int i, j, c;
	struct deal d;
	struct futureTricks fut;
	char str[100];

	/* 0-3, 0=North, 1=East, 2=South, 3=West , Leading hand for the trick.*/
	seat leader = b->n_played_cards % 4 == 0 ? b->current_lead :
		b->played_cards_seat[b->n_played_cards - (b->n_played_cards % 4)];
	d.first = (leader + 2) % 4;
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
	}

	int h, t;
	int tricks[4][5];
	for (t = 0; t < 5; t++) {
		for (h = 0; h < 4; h++) {
			d.trump = dds_suit_conv(t);
			d.first = h;
			i = SolveBoard(d, -1, 1, 0, &fut);
			if (i <= 0) {
				GtkStatusbar *statusbar;
				statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
				if (!status_id)
					status_id = gtk_statusbar_get_context_id(statusbar, "solve");
				snprintf(str, 99, "DD Error: %s", dds_error[-i]);
				gtk_statusbar_push(statusbar, status_id, str);
				return;
			}
			tricks[h][t] = 13 - fut.score[0];
			printf("t %d; h %d = %d\n", t, h, 13 - fut.score[0]);
		}
	}
	system("dds -tricks dd&");

	b->par_score = 0;

	for (i = 1; i <= 7; i++) {
		for (t = club; t <= NT; t++) {
			// FIXME: beide partner betrachten
			int sc = score(i, t, -1, b->vuln[0], tricks[1][t]);
			printf("%s %d\n", contract_string(i, t, north, 0), sc);
			if (sc > b->par_score) {
				b->par_score = sc;
				b->par_level = i;
				b->par_suit = t;
				b->par_dec = north;
				b->par_tricks = tricks[1][t];
				printf("  new par\n");
			}
			sc = -score(i, t, -1, b->vuln[1], tricks[0][t]);
			printf("%s %d\n", contract_string(i, t, east, 0), sc);
			if (sc < b->par_score) {
				b->par_score = sc;
				b->par_level = i;
				b->par_suit = t;
				b->par_dec = east;
				b->par_tricks = tricks[0][t];
				printf("  new par\n");
			}
		}
	}

	GString *par = g_string_new("Par: PASS\n");
	if (b->par_score != 0)
		g_string_printf(par, "Par: %d %s %s\n",
			b->par_score,
			contract_string(b->par_level, b->par_suit, b->par_dec,
				b->par_tricks < b->par_level + 6),
			overtricks(b->par_tricks - b->par_level - 6));

	char *trump_str[] = {"♣", "♦", "♥", "♠", "NT"};
	for (t = 4; t >= 0; t--) {
		g_string_append_printf(par, "%s: ", trump_str[t]);
		if (tricks[1][t] == tricks[3][t])
			g_string_append_printf(par, "NS%d ", tricks[1][t]);
		else
			g_string_append_printf(par, "N%dS%d ", tricks[3][t], tricks[1][t]);
		if (tricks[0][t] == tricks[2][t])
			g_string_append_printf(par, "EW%d", tricks[0][t]);
		else
			g_string_append_printf(par, "E%dW%d", tricks[2][t], tricks[0][t]);
		if (t >= 0)
			g_string_append_printf(par, "\n");
	}

	GtkLabel *par_label = lookup_widget(b->win, "par_label");
	gtk_label_set_markup(par_label, par->str);
	g_string_free(par, TRUE);
}
