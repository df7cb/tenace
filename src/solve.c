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
	gtk_statusbar_pop(statusbar, id);

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

	//SolveBoard(d, int target, int solutions,  int mode, struct futureTricks *futp);
	i = SolveBoard(d, -1, 3, 1, &fut);
	if (i <= 0) {
		snprintf(str, 99, "DD Error: %s", dds_error[-i]);
		gtk_statusbar_push(statusbar, id, str);
		return;
	}
	printf("solve nodes: %d cards: %d\n", fut.nodes, fut.cards);
	for (i = 0; i < fut.cards; i++) {
		c = 13 * (3 - fut.suit[i]) + fut.rank[i] - 2;
		printf("card: %d %d %s (%d) = %d\n", fut.suit[i], fut.rank[i], card_string(c)->str, fut.equals[i], fut.score[i]);
		char *color = (fut.score[i] >= 7 - b->level) ^
			((b->current_lead % 2) != (b->declarer %2)) ? "green" : "red";
		snprintf(str, 99, "<span background=\"%s\"%s>%s</span>",
			color,
			fut.score[i] == fut.score[0] ? " weight=\"bold\"" : "",
			rank_string(fut.rank[i]-2));
		label_set_markup(c, str);
		for (j = 0; j < 13; j++) { /* equals */
			if (fut.equals[i] & card_bits[j]) {
				snprintf(str, 99, "<span background=\"%s\"%s>%s</span>",
					color,
					fut.score[i] == fut.score[0] ? " weight=\"bold\"" : "",
					rank_string(j));
				label_set_markup(13 * (3 - fut.suit[i]) + j, str);
			}
		}
	}

	assert(fut.cards);
	snprintf(str, 99, "DD: %s %+d",
		contract_string(b->level, b->trumps, b->declarer, b->doubled),
		7 - b->level - fut.score[0]);
	gtk_statusbar_push(statusbar, id, str);
}
