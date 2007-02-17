#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "bridge.h"
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
}

void board_reset(board *b)
{
	int i;
	for (i = 0; i < b->n_played_cards; i++)
		give_card(b, b->played_cards_seat[i], b->played_cards[i]);
	memset(b->played_cards, 0, sizeof(card) * 52);
	memset(b->played_cards_seat, 0, sizeof(seat) * 52);
	memset(b->card_score, -1, sizeof(int) * 52);
	b->current_lead = b->declarer == south ? west : b->declarer + 1;

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
	board_reset(b);
	calculate_target(b);
	b->par_score = 0;
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
		free(b->hands[i]);
	}
}

char *rank_string (rank r)
{
	static char *label[] = {"2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K", "A"};
	return label[r];
}

char rank_char (rank r)
{
	static char label[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
	return label[r];
}

rank parse_rank_char (char c)
{
	switch (tolower(c)) {
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return tolower(c) - '2';
		case 't':
			return card10;
		case 'j':
			return cardJ;
		case 'q':
			return cardQ;
		case 'k':
			return cardK;
		case 'a':
			return cardA;
	}
	return -1;
}

GString *card_string (card c)
{
	gchar *suit = NULL;
	static GString *s = NULL;
	if (!s)
		s = g_string_new(NULL);

	switch (SUIT(c)) {
		case spade: suit = "♠"; break;
		case heart: suit = "♥"; break;
		case diamond: suit = "♦"; break;
		case club: suit = "♣"; break;
	}
	g_string_printf(s, "%s%s", suit, rank_string(RANK(c)));
	return s;
}

char *seat_string (seat s)
{
	char *str[] = {0, "West", "North", "East", "South"};
	return str[s];
}

GString *hand_string (hand *h)
{
	static GString *s = NULL;
	if (!s)
		s = g_string_new(NULL);
	assert(h);
	card *c = h->cards;
	g_string_printf(s, "♠");
	while (*c >= 0 && SUIT(*c) == spade)
		g_string_append_printf(s, "%s", rank_string(RANK(*c++)));
	g_string_append_printf(s, "\n♥");
	while (*c >= 0 && SUIT(*c) == heart)
		g_string_append_printf(s, "%s", rank_string(RANK(*c++)));
	g_string_append_printf(s, "\n♦");
	while (*c >= 0 && SUIT(*c) == diamond)
		g_string_append_printf(s, "%s", rank_string(RANK(*c++)));
	g_string_append_printf(s, "\n♣");
	while (*c >= 0 && SUIT(*c) == club)
		g_string_append_printf(s, "%s", rank_string(RANK(*c++)));
	return s;
}

GString *gib_string (hand *h)
{
	static GString *s = NULL;
	char rank_char[] = {'2', '3', '4', '5', '6', '7', '8', '9', 't', 'j', 'q', 'k', 'a'};
	if (!s)
		s = g_string_new(NULL);
	assert(h);
	card *c = h->cards;
	g_string_truncate(s, 0);
	while (*c >= 0 && SUIT(*c) == spade)
		g_string_append_printf(s, "%c", rank_char[RANK(*c++)]);
	g_string_append_printf(s, ".");
	while (*c >= 0 && SUIT(*c) == heart)
		g_string_append_printf(s, "%c", rank_char[RANK(*c++)]);
	g_string_append_printf(s, ".");
	while (*c >= 0 && SUIT(*c) == diamond)
		g_string_append_printf(s, "%c", rank_char[RANK(*c++)]);
	g_string_append_printf(s, ".");
	while (*c >= 0 && SUIT(*c) == club)
		g_string_append_printf(s, "%c", rank_char[RANK(*c++)]);
	return s;
}

int has_suit(hand *h, suit s)
{
	card *p;
	for (p = h->cards; *p >= 0; p++)
		if (SUIT(*p) == s)
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
	seat cs = b->cards[c];
	//printf("s%d c%d cs%d\n", s, c, cs);
	assert (cs >= 0 && cs <= 4);
	if (cs == s) { /* remove card from this hand */
		remove_card(b->hands[s-1], c);
		b->cards[c] = 0;
		return 0;
	}
	if (b->hands[s-1]->cards[12] >= 0) /* hand has already 13 cards */
		return cs != 0;
	if (cs) /* someone else has the card, remove it */
		remove_card(b->hands[cs-1], c);
	add_card(b->hands[s-1], c); /* add it here */
	b->cards[c] = s;
	return 1;
}

char *contract_string(int level, suit trumps, seat declarer, int doubled)
{
	static char buf[20];
	char *trump_str[] = {"♣", "♦", "♥", "♠", "NT"};
	char *declarer_str[] = {0, "W", "N", "E", "S"};
	sprintf(buf, "%d%s %s%s", level, trump_str[trumps], declarer_str[declarer],
		doubled == 2 ? " XX" :
			doubled == 1 ? " X" : "");
	return buf;
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
	if (b->cards[c] != b->current_lead) {
		gtk_statusbar_push(statusbar, id, "Not your turn");
		return 0;
	}

	int firstcard;
	card lead;
	if (b->n_played_cards % 4 != 0) { /* must follow suit */
		firstcard = b->n_played_cards - (b->n_played_cards % 4);
		lead = b->played_cards[firstcard];
		if (SUIT(c) != SUIT(lead) && has_suit(b->hands[s - 1], SUIT(lead))) {
			gtk_statusbar_push(statusbar, id, "Please follow suit");
			return 0;
		}
	}

	i = give_card(b, s, c);
	assert(i == 0);
	b->played_cards[b->n_played_cards] = c;
	b->played_cards_seat[b->n_played_cards] = s;

	char *labels[] = {0, "card_west", "card_north", "card_east", "card_south"};
	if (b->n_played_cards % 4 == 0)
		for (i = west; i <= south; i++) {
			GtkWidget *label = lookup_widget(b->win, labels[i]);
			gtk_label_set_text(GTK_LABEL(label), "");
		}
	GtkWidget *label = lookup_widget(b->win, labels[s]);
	gtk_label_set_text(GTK_LABEL(label), card_string(c)->str);

	if (b->n_played_cards % 4 == 3) { /* trick complete */
		seat leader = b->played_cards_seat[firstcard];
		card wincard = lead;
		b->current_lead = leader;
		for (i = 1; i <= 3; i++) {
			card thiscard = b->played_cards[firstcard + i];
			if ((SUIT(thiscard) == b->trumps && SUIT(wincard) != b->trumps) ||
			    (SUIT(thiscard) == SUIT(wincard) && RANK(thiscard) > RANK(wincard))) {
				wincard = thiscard;
				b->current_lead = leader + i > south ? leader + i - 4 : leader + i;
			}
		}
		b->tricks[b->current_lead % 2]++;
	} else {
		b->current_lead = b->current_lead == south ? west : b->current_lead + 1;
	}

	b->n_played_cards++;

	return 1;
}

char *overtricks (int i)
{
	static char buf[4];
	if (i == 0)
		snprintf(buf, 3, "=");
	else
		snprintf(buf, 3, "%+d", i);
	return buf;
}

int score (int level, suit s, int doubled, int vuln, int tricks)
{
	/* doubled: 0 / 1 (double) / 2 (FIXME) / -1 (auto-double down contracts) */
	int nv_double[] = { 100, 300, 500, 800, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3500 };
	int v_double[] = { 200, 500, 800, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3500, 3800 };
	if (tricks < level + 6) {
		if (doubled != 0) {
			if (vuln) {
				return -v_double[level + 5 - tricks];
			} else {
				return -nv_double[level + 5 - tricks];
			}
		} else {
			return (tricks - level - 6) * 50 * (vuln + 1);
		}
	} else {
		if (doubled < 0)
			doubled = 0;
		// FIXME: notrump
		int trick_score = ((s >= heart) ? 30 : 20) * (doubled + 1);
		int overtricks = tricks - level - 6;

		int base_score = level * trick_score;
		int overtrick_score = overtricks * (doubled ? 100 * (vuln + 1) : trick_score);
		int game_bonus = vuln ? 450 : 250;
		return 50 * (doubled + 1) + base_score + overtrick_score
			+ (base_score >= 100 ? game_bonus : 0)
			+ (level >= 6 ? (vuln ? 750 : 500) : 0)
			+ (level == 7 ? (vuln ? 750 : 500) : 0);
	}
}

