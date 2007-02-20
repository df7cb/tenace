#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "functions.h"
#include "support.h"

void board_statusbar(GtkWidget *win, char *text)
{
	GtkStatusbar *statusbar;
	statusbar = GTK_STATUSBAR(lookup_widget(win, "statusbar1"));
	static guint id = 0;
	if (!id)
		id = gtk_statusbar_get_context_id(statusbar, "bridge_c");
	gtk_statusbar_pop(statusbar, id);

	if (text)
		gtk_statusbar_push(statusbar, id, text);
}

void calculate_target(board *b)
{
	int side = b->declarer % 2;
	b->target[side] = b->level + 6;
	b->target[1 - side] = 14 - b->target[side]; /* 1 more to beat contract */
}

void board_clear(board *b)
{
	b->n_dealt_cards = b->n_played_cards = 0;
	int i;
	for (i = 0; i < 52; i++) {
		b->cards[i] = 0;
		b->dealt_cards[i] = 0;
		b->card_score[i] = -1;
		b->played_cards[i] = -1;
	}
	for (i = 0; i < 4; i++)
		b->hand_cards[i] = 0;
	b->current_turn = seat_mod(b->declarer + 1);

	b->par_score = -1;
	b->par_dec = b->par_suit = b->par_level = b->par_tricks = 0;
}

void board_set_contract(board *b, int level, suit trump, seat declarer, int doubled)
{
	b->level = level;
	b->trumps = trump;
	b->declarer = declarer;
	b->current_turn = seat_mod(declarer + 1);
	b->doubled = doubled;
	calculate_target(b);
}

board *board_new(void)
{
	int i;
	char *names[] = {"West", "North", "East", "South"};
	board *b = malloc(sizeof(board));
	assert(b);
	b->name = g_string_new("Board 1");
	for (i = 0; i < 4; i++) {
		b->hand_name[i] = g_string_new(names[i]);
	}

	board_set_contract(b, 1, NT, south, 0);
	board_clear(b);
	b->vuln[0] = b->vuln[1] = 0;

	return b;
}

void board_free(board *b)
{
	int i;
	assert(b);
	g_string_free(b->name, TRUE);
	for (i = 0; i < 4; i++) {
		g_string_free(b->hand_name[i], TRUE);
	}
}

/* dealing with cards */

int assert_board(board *b) /* check proper number of cards in hands */
{
	int i;
	for (i = 1; i < 4; i++)
		if (b->hand_cards[0] != b->hand_cards[i])
			return 0;
	return 1;
}

int add_card(board *b, seat s, card c)
/* return: 1 = card added */
{
	if (b->dealt_cards[c] != 0)
		return 0;
	if (b->hand_cards[s-1] == 13)
		return 0;

	b->cards[c] = s;
	b->dealt_cards[c] = s;
	b->n_dealt_cards++;
	b->hand_cards[s-1]++;

	b->par_score = -1;

	return 1;
}

int remove_card(board *b, seat s, card c)
/* return: 1 = card removed */
{
	assert (b->dealt_cards[c] == s);

	b->cards[c] = 0;
	b->dealt_cards[c] = 0;
	b->n_dealt_cards--;
	b->hand_cards[s-1]--;

	b->par_score = -1;

	return 1;
}

void deal_random(board *b)
{
	seat s;
	for (s = west; s <= south; s++) {
		while (b->hand_cards[s-1] < 13) {
			int c = rand() % 52;
			if (b->dealt_cards[c] == 0) {
				int ret = add_card(b, s, c);
				assert(ret);
			}
		}
	}
}

static int has_suit(seat *cards, seat h, suit s)
{
	int i;
	for (i = 0; i < 52; i++)
		if (SUIT(i) == s && cards[i] == h)
			return 1;
	return 0;
}

static void play_card_0(board *b, seat s, card c)
{
	assert (s);
	assert (b->dealt_cards[c] == s);
	assert (b->cards[c] == s);

	b->cards[c] = 0;

	b->played_cards[b->n_played_cards] = c;
	b->n_played_cards++;
}

int play_card(board *b, seat s, card c)
{
	board_statusbar(b->win, NULL);

	if (b->cards[c] != b->current_turn) {
		board_statusbar(b->win, "Not your turn");
		return 0;
	}

	int firstcard;
	card lead;
	if (b->n_played_cards % 4 != 0) { /* must follow suit */
		firstcard = b->n_played_cards - (b->n_played_cards % 4);
		lead = b->played_cards[firstcard];
		if (SUIT(c) != SUIT(lead) && has_suit(b->cards, s, SUIT(lead))) {
			board_statusbar(b->win, "Please follow suit");
			return 0;
		}
	}

	play_card_0(b, s, c);

	if (b->n_played_cards % 4 == 0) { /* trick complete */
		seat leader = b->dealt_cards[lead];
		card wincard = lead;
		b->current_turn = leader;
		int i;
		for (i = 1; i <= 3; i++) {
			card thiscard = b->played_cards[firstcard + i];
			if ((SUIT(thiscard) == b->trumps && SUIT(wincard) != b->trumps) ||
			    (SUIT(thiscard) == SUIT(wincard) && RANK(thiscard) > RANK(wincard))) {
				wincard = thiscard;
				b->current_turn = seat_mod(leader + i);
			}
		}
		b->tricks[b->current_turn % 2]++;
	} else {
		b->current_turn = seat_mod(b->current_turn + 1);
	}

	return 1;
}

int rewind_card(board *b)
{
	if (b->n_played_cards == 0) {
		board_statusbar(b->win, "Nothing to undo");
		return 0;
	}

	b->n_played_cards--;

	if (b->n_played_cards % 4 == 3)
		b->tricks[b->current_turn % 2]--;

	card c = b->played_cards[b->n_played_cards];
	assert (b->cards[c] == 0);
	b->current_turn = b->cards[c] = b->dealt_cards[c];

	return 1;
}

void board_rewind(board *b)
{
	while (b->n_played_cards)
		rewind_card(b);
}

int next_card(board *b)
{
	if (b->n_played_cards >= b->n_dealt_cards) {
		board_statusbar(b->win, "No cards left to play");
		return 0;
	}
	if (b->played_cards[b->n_played_cards] == -1) {
		board_statusbar(b->win, "Which card should I play?");
		return 0;
	}
	if (b->cards[b->played_cards[b->n_played_cards]] == 0) {
		board_statusbar(b->win, "Card was already played");
		return 0;
	}
	if (b->cards[b->played_cards[b->n_played_cards]] != b->current_turn) {
		board_statusbar(b->win, "Card belongs to wrong player");
		return 0;
	}
	return play_card(b, b->current_turn, b->played_cards[b->n_played_cards]);
}

void board_fast_forward(board *b)
{
	while (next_card(b));
}
