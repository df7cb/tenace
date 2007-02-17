#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "bridge.h"
#include "functions.h"
#include "support.h"

void hand_clear(hand *h)
{
	//g_string_truncate(h->name, 0);
	memset(h->cards, -1, sizeof(card) * 14);
	h->ncards = 0;
}

hand *hand_new(char *name)
{
	hand *h = malloc(sizeof(hand));
	assert(h);
	h->name = g_string_new(name);
	hand_clear(h);
	return h;
}

void hand_free(hand *h)
{
	assert(h);
	g_string_free(h->name, TRUE);
	free(h);
}

void board_clear_cards(board *b)
{
	memset(b->cards, 0, sizeof(seat) * 52);
	b->n_dealt_cards = 0;
	memset(b->dealt_cards, 0, sizeof(seat) * 52);
}

void board_rewind(board *b)
{
	int i;
	for (i = 0; i < b->n_played_cards; i++)
		give_card(b, b->played_cards_seat[i], b->played_cards[i]);
	memset(b->played_cards, 0, sizeof(card) * 52);
	memset(b->played_cards_seat, 0, sizeof(seat) * 52);
	memset(b->card_score, -1, sizeof(int) * 52);
	b->current_turn = seat_mod(b->declarer + 1);

	b->tricks[0] = b->tricks[1] = 0;
}

void calculate_target(board *b)
{
	int side = b->declarer % 2;
	b->target[side] = b->level + 6;
	b->target[1 - side] = 14 - b->target[side]; /* 1 more to beat contract */
}

board *board_new(void)
{
	int i;
	char *names[] = {"West", "North", "East", "South"};
	board *b = malloc(sizeof(board));
	assert(b);
	b->name = g_string_new("Board 1");
	for (i = 0; i < 4; i++) {
		b->hands[i] = hand_new(names[i]);
	}
	board_clear_cards(b);
	b->trumps = NT;
	b->declarer = south;
	b->level = 1;
	b->doubled = 0;
	b->vuln[0] = b->vuln[1] = 0;
	b->n_played_cards = 0;
	memset(b->hand_cards, 0, sizeof(int) * 4);
	board_rewind(b);
	calculate_target(b);
	b->par_score = -1;
	b->par_dec = b->par_suit = b->par_level = b->par_tricks = 0;
	//memset(b->card_label, 0, sizeof(GtkWidget*) * 52);
	return b;
}

void board_clear(board *b)
{
	int i;
	for (i = 0; i < 4; i++) {
		hand_clear(b->hands[i]);
	}
	board_clear_cards(b);
}

void board_free(board *b)
{
	int i;
	assert(b);
	g_string_free(b->name, TRUE);
	for (i = 0; i < 4; i++) {
		hand_free(b->hands[i]);
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

void remove_card(hand *h, card c)
{
	card *p = h->cards;
	while (*p >= 0 && *p != c)
		p++;
	while (*p >= 0) {
		*p = *(p + 1);
		p++;
	}
	h->ncards--;
}

static void add_card(hand *h, card c)
/* add card to hand and keep list sorted */
{
	card *p = h->cards;
	while (*p >= 0)
		p++;
	*p = c;
	*(p+1) = -1;
	do {
		if (*p < *(p+1)) {
			card tmp = *(p+1);
			*(p+1) = *p;
			*p = tmp;
		}
		if (p == h->cards)
			break;
		p--;
	} while (1);
	h->ncards++;
}

int give_card(board *b, seat s, card c)
/* return: 1 = card added, 0 = card removed */
{
	assert (c >= 0 && c < 52);
	seat cs = b->dealt_cards[c];
	//printf("s%d c%d cs%d\n", s, c, cs);
	assert (cs >= 0 && cs <= 4);
	if (cs == s) { /* remove card from this hand */
		remove_card(b->hands[s-1], c);
		b->cards[c] = 0;
		b->dealt_cards[c] = 0;
		b->hand_cards[s-1]--;
		return 0;
	}
	if (b->hands[s-1]->cards[12] >= 0) /* hand has already 13 cards */
		return cs != 0;
	if (cs) {/* someone else has the card, remove it */
		remove_card(b->hands[cs-1], c);
		b->hand_cards[cs-1]--;
	}
	add_card(b->hands[s-1], c); /* add it here */
	b->cards[c] = s;
	b->dealt_cards[c] = s;
	b->hand_cards[s-1]++;
	return 1;
}

int play_card(board *b, seat s, card c)
{
	GtkStatusbar *statusbar;
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	static guint id = 0;
	if (!id)
		id = gtk_statusbar_get_context_id(statusbar, "play_card");
	gtk_statusbar_pop(statusbar, id);

	int i;
	if (b->cards[c] != b->current_turn) {
		gtk_statusbar_push(statusbar, id, "Not your turn");
		return 0;
	}

	int firstcard;
	card lead;
	if (b->n_played_cards % 4 != 0) { /* must follow suit */
		firstcard = b->n_played_cards - (b->n_played_cards % 4);
		lead = b->played_cards[firstcard];
		if (SUIT(c) != SUIT(lead) && has_suit(b->cards, s, SUIT(lead))) {
			gtk_statusbar_push(statusbar, id, "Please follow suit");
			return 0;
		}
	}

	i = give_card(b, s, c);
	assert(i == 0);
	b->played_cards[b->n_played_cards] = c;
	b->played_cards_seat[b->n_played_cards] = s;

	show_played_card(b, s, c);

	if (b->n_played_cards % 4 == 3) { /* trick complete */
		seat leader = b->played_cards_seat[firstcard];
		card wincard = lead;
		b->current_turn = leader;
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

	b->n_played_cards++;

	return 1;
}

