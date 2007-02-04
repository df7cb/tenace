#include <assert.h>
#include <dds.h>
#include "support.h" /* lookup_widget */
#include "board.h"
#include "main.h"

void init_solve()
{
	InitStart();
}

char *dds_error[] = {
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

void solve_board(board *b)
{
	int i, j, c;
	struct deal d;
	struct futureTricks fut;
	const int card_bits[] = {0x4, 0x8, 0x10, 0x20, 0x40, 0x80, 0x100, 0x200, 0x400, 0x800, 0x1000, 0x2000, 0x4000};
	char str[100];

	GtkStatusbar *statusbar;
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	static guint id = 0;
	if (!id)
		id = gtk_statusbar_get_context_id(statusbar, "solve");

	for (i = north; i <= south; i++)
		if (b->hands[i-1]->ncards != b->hands[west-1]->ncards) {
			snprintf(str, 99, "Error: %s has %d cards while %s has %d",
				seat_string(west), b->hands[west-1]->ncards,
				seat_string(i), b->hands[i-1]->ncards);
			gtk_statusbar_push(statusbar, id, str);
			return;
		}

	d.trump = b->trumps == NT ? NT : 3 - b->trumps;
	/* 0-3, 0=North, 1=East, 2=South, 3=West , Leading hand for the trick.*/
	d.first = (b->current_lead + 2) % 4;
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

	gtk_statusbar_push(statusbar, id, "Thinking...");
	gtk_main_iteration_do(TRUE); // TODO: fixme
	i = SolveBoard(d, -1, 3, 1, &fut);
	if (i <= 0) {
		snprintf(str, 99, "DD Error: %s", dds_error[-i]);
		gtk_statusbar_push(statusbar, id, str);
		return;
	}
	gtk_statusbar_pop(statusbar, id);
	printf("solve nodes: %d cards: %d\n", fut.nodes, fut.cards);

	int side = b->current_lead % 2;
	printf("%d has %d tricks, needs %d\n", side, b->tricks[side], b->target[side]);
	for (i = 0; i < fut.cards; i++) {
		c = 13 * (3 - fut.suit[i]) + fut.rank[i] - 2;
		printf("card: %s (%x) = %d\n", card_string(c)->str, fut.equals[i], fut.score[i]);
		b->card_score[c] = fut.score[i];

		char *color = b->tricks[side] + fut.score[i] >= b->target[side] ? "green" : "red";
		char *weight = fut.score[i] == fut.score[0] ? " weight=\"bold\"" : "";
		snprintf(str, 99, "<span background=\"%s\"%s>%s</span>",
			color, weight, rank_string(fut.rank[i]-2));
		label_set_markup(c, str);
		for (j = fut.rank[i] - 2; j >= 0; j--) { /* equals */
			if (fut.equals[i] & card_bits[j]) {
				c = 13 * (3 - fut.suit[i]) + j;
				b->card_score[c] = fut.score[i];

				snprintf(str, 99, "<span background=\"%s\"%s>%s</span>",
					color, weight, rank_string(j));
				label_set_markup(c, str);
			}
		}
	}

	assert(fut.cards);
	snprintf(str, 99, "DD: %s %+d",
		contract_string(b->level, b->trumps, b->declarer, b->doubled),
		7 - b->level - fut.score[0]);
	gtk_statusbar_push(statusbar, id, str);
}
