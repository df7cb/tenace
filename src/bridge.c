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
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "functions.h"
#include "support.h"
#include "window_board.h"

/* bridge math */

void calculate_target(board *b)
{
	int side = b->declarer % 2;
	b->target[side] = b->level + 6;
	b->target[1 - side] = 14 - b->target[side]; /* 1 more to beat contract */
}

int
card_overtricks (board *b, card c)
{
	assert (b->current_dd && b->current_dd->card_score[c] >= 0);

	return b->current_dd->card_score[c] - 6 - b->level;
}

int
card_is_good (board *b, card c)
{
	assert (b->current_dd && b->current_dd->card_score[c] >= 0);

	int side = b->current_turn % 2;
	if (side == (b->declarer % 2))
		return b->current_dd->card_score[c] >= b->target[side];
	else
		return 13 - b->current_dd->card_score[c] >= b->target[side];

	/* NOT REACHED */
}

/* dealing with boards */

void board_clear(board *b)
{
	b->n_dealt_cards = b->n_played_cards = 0;
	int i;
	for (i = 0; i < 52; i++) {
		b->cards[i] = 0;
		b->dealt_cards[i] = 0;
		if (b->next_dd[i]) {
			free (b->next_dd[i]);
			b->next_dd[i] = NULL;
		}
		b->played_cards[i] = -1;
	}
	for (i = 0; i < 4; i++)
		b->hand_cards[i] = 0;
	b->current_turn = seat_mod(b->declarer + 1);
	b->tricks[0] = b->tricks[1] = 0;

	if (b->current_dd) {
		free (b->current_dd);
		b->current_dd = NULL;
	}
	b->par_score = -1;
	b->par_dec = b->par_suit = b->par_level = b->par_tricks = 0;
}

void board_set_contract(board *b, int level, suit trumps, seat declarer, int doubled)
{
	b->level = level;
	b->trumps = trumps;
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
	b->n = 0; /* will be set by the board menu hander */
	for (i = 0; i < 4; i++) {
		b->hand_name[i] = g_string_new (_(names[i]));
	}

	b->dealer = north;
	board_set_contract(b, 1, NT, south, 0);
	b->current_dd = NULL;
	for (i = 0; i < 52; i++) {
		b->next_dd[i] = NULL;
	}
	board_clear(b);
	b->vuln[0] = b->vuln[1] = 0;

	b->bidding = calloc(4, sizeof(card));
	assert(b->bidding);
	b->alerts = calloc(4, sizeof(char *));
	assert(b->alerts);
	b->n_bids = 0;
	b->n_bid_alloc = 4;

	return b;
}

board *
board_dup (board *b0)
{
	int i;
	board *b = malloc(sizeof(board));
	assert(b);
	*b = *b0;

	b->name = g_string_new(b0->name->str);
	for (i = 0; i < 4; i++) {
		b->hand_name[i] = g_string_new (b0->hand_name[i]->str);
	}

	b->current_dd = NULL;
	for (i = 0; i < 52; i++) {
		b->next_dd[i] = NULL;
	}

	b->bidding = calloc(b0->n_bid_alloc, sizeof(card));
	assert(b->bidding);
	b->alerts = calloc(b0->n_bid_alloc, sizeof(char *));
	assert(b->alerts);

	for (i = 0; i < b0->n_bids; i++) {
		board_append_bid (b, b0->bidding[i], 0);
		board_set_alert (b, b0->alerts[i]);
	}

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
	free(b->bidding);
	for (i = 0; i < b->n_bid_alloc; i++)
		if (b->alerts[i])
			free (b->alerts[i]);
	free (b->alerts);
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

int
flip_hands (board *b, seat h1, seat h2)
{
	int cards[13];
	int n_cards = 0;
	int c, i;
	for (c = 0; c < 52; c++) {
		if (b->cards[c] == h1) {
			cards[n_cards++] = c;
			remove_card (b, h1, c);
		}
	}
	for (c = 0; c < 52; c++) {
		if (b->cards[c] == h2) {
			remove_card (b, h2, c);
			if (!add_card (b, h1, c)) {
				add_card (b, h2, c);
				break;
			}
		}
	}
	for (i = 0; i < n_cards; i++) {
		if (!add_card (b, h2, cards[i]))
			break;
		cards[i] = -1;
	}
	for (i = 0; i < n_cards; i++) {
		if (cards[i] >= 0) {
			c = add_card (b, h1, cards[i]);
			assert (c);
		}
	}
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

/* playing */

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
	board_statusbar(NULL);

	if (b->cards[c] == 0) {
		board_statusbar(_("Card is not in your hand"));
		return 0;
	}

	if (b->cards[c] != b->current_turn) {
		board_statusbar(_("Not your turn"));
		return 0;
	}

	int firstcard = 0;
	card lead = 0;
	if (b->n_played_cards % 4 != 0) { /* must follow suit */
		firstcard = b->n_played_cards - (b->n_played_cards % 4);
		lead = b->played_cards[firstcard];
		if (SUIT(c) != SUIT(lead) && has_suit(b->cards, s, SUIT(lead))) {
			board_statusbar(_("Please follow suit"));
			return 0;
		}
	}

	int old_next_card = b->played_cards[b->n_played_cards];

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

	/* update DD scores */
	dd_t *dd = b->next_dd[c];
	int i;
	for (i = 0; i < 52; i++) { /* remove the other DD previews */
		if (b->next_dd[i] && b->cards[i] && (b->cards[i] == s || c != old_next_card)) {
			/* invalidate "what-if" previews and old scores from other cards */
			free (b->next_dd[i]);
			b->next_dd[i] = NULL;
		}
	}
	b->next_dd[c] = b->current_dd; /* remember scores for rewind */
	b->current_dd = dd;

	/*
	printf ("previews for: ");
	for (i = 0; i < 52; i++)
		if (b->next_dd[i])
			printf ("%s ", card_string(i)->str);
	printf ("\n");
	*/

	return 1;
}

int rewind_card(board *b)
{
	if (b->n_played_cards == 0) {
		board_statusbar(_("Nothing to undo"));
		return 0;
	}

	if (b->n_played_cards % 4 == 0)
		b->tricks[b->current_turn % 2]--;
	b->n_played_cards--;

	card c = b->played_cards[b->n_played_cards];
	assert (b->cards[c] == 0);
	seat s = b->dealt_cards[c];

	/* remove DD previews */
	dd_t *dd = b->next_dd[c];
	int i;
	for (i = 0; i < 52; i++) {
		if (b->next_dd[i] && b->cards[i] == s) {
			free (b->next_dd[i]);
			b->next_dd[i] = NULL;
		}
	}
	b->next_dd[c] = b->current_dd;
	b->current_dd = dd;

	/* return card */
	b->current_turn = b->cards[c] = s;
	/*
	printf ("previews for: ");
	for (i = 0; i < 52; i++)
		if (b->next_dd[i])
			printf ("%s ", card_string(i)->str);
	printf ("\n");
	*/

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
		board_statusbar(_("No cards left to play"));
		return 0;
	}
	if (b->played_cards[b->n_played_cards] == -1) {
		board_statusbar(_("Which card should I play?"));
		return 0;
	}
	if (b->cards[b->played_cards[b->n_played_cards]] == 0) {
		board_statusbar(_("Card was already played"));
		return 0;
	}
	if (b->cards[b->played_cards[b->n_played_cards]] != b->current_turn) {
		board_statusbar(_("Card belongs to wrong player"));
		return 0;
	}
	return play_card(b, b->current_turn, b->played_cards[b->n_played_cards]);
}

void board_fast_forward(board *b)
{
	while (b->n_played_cards < b->n_dealt_cards &&
			b->played_cards[b->n_played_cards] != -1)
		next_card(b);
}

/* bidding */

int
board_append_bid(board *b, card bid, int set_contract)
{
	int cur_bid = 0, cur_bid_i = 0, n_pass = 0, doubl = 0, redoubl = 0;
	int i;
	for (i = b->n_bids - 1; i >= 0; i--) {
		if (b->bidding[i] == bid_pass && !doubl && !redoubl)
			n_pass++; /* number of trailing passes */
		if (b->bidding[i] == bid_x)
			doubl = 1;
		if (b->bidding[i] == bid_xx)
			redoubl = 1;
		if (b->bidding[i] > bid_xx) {
			cur_bid = b->bidding[i];
			cur_bid_i = i;
			break;
		}
	}

	if (n_pass == 4 || (n_pass == 3 && cur_bid)) {
		board_statusbar(_("Bidding is over"));
		return 0;
	}

	if (bid == bid_x)
		if (doubl || redoubl || cur_bid == 0 || n_pass % 2 != 0) {
			board_statusbar(_("Invalid double"));
			return 0;
		}

	if (bid == bid_xx)
		if (!doubl || redoubl || cur_bid == 0 || n_pass % 2 != 0) {
			board_statusbar(_("Invalid redouble"));
			return 0;
		}

	if (bid > bid_xx && bid <= cur_bid) {
		board_statusbar(_("Insufficient bid"));
		return 0;
	}

	if (b->n_bids >= b->n_bid_alloc) {
		b->n_bid_alloc <<= 2;
		b->bidding = realloc(b->bidding, b->n_bid_alloc * sizeof (card));
		b->alerts = realloc(b->alerts, b->n_bid_alloc * sizeof (char *));
		assert(b->bidding && b->alerts);
		int i; /* I wished there was recalloc() */
		for (i = b->n_bid_alloc >> 2; i < b->n_bid_alloc; i++)
			b->alerts[i] = NULL;
	}

	b->bidding[b->n_bids] = bid;
	b->alerts[b->n_bids] = NULL;
	b->n_bids++;

	if (bid == bid_pass && (n_pass == 3 || (n_pass == 2 && cur_bid))) {
		if (set_contract) {
			int trumps = DENOM (cur_bid);
			int declarer = 0;
			if (cur_bid) { /* find declarer */
				for (i = cur_bid_i % 2; i < b->n_bids; i += 2)
					if (DENOM (b->bidding[i]) == trumps) {
						declarer = seat_mod (b->dealer + i);
						break;
					}
				assert (declarer); /* someone must actually bid the contract */
			} else {
				declarer = b->dealer; /* default for passed boards */
			}
			board_set_contract (b, LEVEL (cur_bid), trumps, declarer, doubl + redoubl);
		}
		return 2; /* signal end of bidding */
	}

	return 1;
}

void
board_remove_bid (board *b)
{
	assert (b->n_bids > 0);
	b->n_bids--;
	if (b->alerts[b->n_bids]) {
		free (b->alerts[b->n_bids]);
		b->alerts[b->n_bids] = NULL;
	}
}

void
board_clear_bidding (board *b)
{
	while (b->n_bids > 0)
		board_remove_bid (b);
}

void
board_set_alert (board *b, const char *alert)
{
	assert (b->n_bids > 0);
	if (b->alerts[b->n_bids - 1])
		free (b->alerts[b->n_bids - 1]);
	if (alert)
		b->alerts[b->n_bids - 1] = strdup (alert);
	else
		b->alerts[b->n_bids - 1] = NULL;
}
