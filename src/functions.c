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
#include <string.h>

#include "bridge.h"

int pos_score_for = 0; /* 0 = NS, 1 = declarer, 2 = current lead */

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
		case 'z':
		case '1':
			return card10;
		case 'j':
		case 'b':
			return cardJ;
		case 'q':
		case 'd':
			return cardQ;
		case 'k':
			return cardK;
		case 'a':
			return cardA;
	}
	return -1;
}

int parse_suit(char c)
{
	switch (tolower(c)) {
		case 'n': return NT; break;
		case 's': return spade; break;
		case 'h': return heart; break;
		case 'd': return diamond; break;
		case 'c': return club; break;
	}
	return -1;
}

int parse_card(char *tok)
{
	if (strlen(tok) != 2)
		return -1;

	int su = parse_suit(tok[0]);
	int ra = parse_rank_char(tok[1]);
	if (su == -1 || ra == -1)
		return -1;
	return 13 * su + ra;
}

int parse_bid(char *tok)
{
	if (!strcasecmp(tok, "p"))
		return bid_pass;
	if (!strcasecmp(tok, "x") || !strcasecmp(tok, "d"))
		return bid_x;
	if (!strcasecmp(tok, "xx") || !strcasecmp(tok, "r"))
		return bid_x;
	if (strlen(tok) != 2)
		return -1;

	int le = tok[0] - '0';
	int su = parse_suit(tok[1]);
	if (le < 1 || le > 7 || su == -1)
		return -1;
	return 5 * le + su;
}

GString *card_string (card c)
{
	char *su = NULL;
	static GString *s = NULL;
	if (!s)
		s = g_string_new(NULL);

	switch (SUIT(c)) {
		case spade: su = "♠"; break;
		case heart: su = "♥"; break;
		case diamond: su = "♦"; break;
		case club: su = "♣"; break;
	}
	g_string_printf(s, "%s%s", su, rank_string(RANK(c)));
	return s;
}

GString *bid_string (card c)
{
	char *su = NULL;
	static GString *s = NULL;
	if (!s)
		s = g_string_new(NULL);

	if (c == bid_pass) {
		g_string_printf(s, "-");
	} else if (c == bid_x) {
		g_string_printf(s, "X");
	} else if (c == bid_x) {
		g_string_printf(s, "XX");
	} else {
		switch (DENOM(c)) {
			case NT: su = "NT"; break;
			case spade: su = "♠"; break;
			case heart: su = "♥"; break;
			case diamond: su = "♦"; break;
			case club: su = "♣"; break;
		}
		g_string_printf(s, "%d%s", LEVEL(c), su);
	}
	return s;
}

char *seat_string (seat s)
{
	char *str[] = {0, "West", "North", "East", "South"};
	return str[s];
}

GString *hand_string (board *b, seat h)
{
	static GString *s = NULL;
	if (!s)
		s = g_string_new(NULL);

	int c;
	g_string_printf(s, "♠");
	for (c = 51; c >= 39; c--)
		if (b->dealt_cards[c] == h)
			g_string_append_printf(s, "%s", rank_string(RANK(c)));
	g_string_append_printf(s, "\n♥");
	for (c = 38; c >= 26; c--)
		if (b->dealt_cards[c] == h)
			g_string_append_printf(s, "%s", rank_string(RANK(c)));
	g_string_append_printf(s, "\n♦");
	for (c = 25; c >= 13; c--)
		if (b->dealt_cards[c] == h)
			g_string_append_printf(s, "%s", rank_string(RANK(c)));
	g_string_append_printf(s, "\n♣");
	for (c = 12; c >= 0; c--)
		if (b->dealt_cards[c] == h)
			g_string_append_printf(s, "%s", rank_string(RANK(c)));
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
	snprintf(buf, 20, "%d%s %s%s", level, trump_str[trumps], declarer_str[declarer],
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
	/* doubled: 0 / 1 (double) / 2 (redouble) / -1 (auto-double down contracts) */
	int nv_double[] = { 100, 300, 500, 800, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3500 };
	int v_double[] = { 200, 500, 800, 1100, 1400, 1700, 2000, 2300, 2600, 2900, 3200, 3500, 3800 };

	if (tricks < level + 6) {
		if (doubled < 0)
			doubled = 1;

		if (doubled) {
			if (vuln) {
				return -v_double[level + 5 - tricks] * doubled;
			} else {
				return -nv_double[level + 5 - tricks] * doubled;
			}
		}
		return (tricks - level - 6) * 50 * (vuln + 1);

	} else {
		if (doubled < 0)
			doubled = 0;

		int trick_score = (s >= heart) ? 30 : 20;
		int base_score = (level * trick_score + (s == NT ? 10 : 0)) * (1 << doubled);

		int per_overtrick = doubled ? 100 * (vuln + 1) * doubled : trick_score;
		int overtrick_score = (tricks - level - 6) * per_overtrick;

		int game_bonus = vuln ? 450 : 250;
		return base_score + overtrick_score
			+ 50 * (1 + doubled)
			+ (base_score >= 100 ? game_bonus : 0)
			+ (level >= 6 ? (vuln ? 750 : 500) : 0)
			+ (level == 7 ? (vuln ? 750 : 500) : 0);
	}
}

char *score_string(int level, suit trumps, seat declarer, int doubled, int vuln, int tricks, seat lead)
{
	static char buf[25];
	int flip = 0;
	switch (pos_score_for) {
		case 0: /* NS */
			flip = (declarer % 2) == 1 ? -1 : 1;
			break;
		case 1: /* declarer */
			flip = 1;
			break;
		case 2: /* current lead */
			flip = (declarer % 2) == (lead % 2) ? 1 : -1;
			break;
	}
	snprintf(buf, 24, "%s %s (%+d)",
		contract_string(level, trumps, declarer, doubled),
		overtricks(tricks - 6 - level),
		flip * score(level, trumps, doubled, vuln, tricks));
	return buf;
}
