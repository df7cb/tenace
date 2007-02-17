#include <assert.h>

#include "bridge.h"

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

/*
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
*/

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

seat seat_mod(seat s)
{
	return ((s + 3) % 4) + 1;
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

