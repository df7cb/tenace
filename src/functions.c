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
#include "functions.h"
#include "support.h"

const char *seat_str[] = {0, "W", "N", "E", "S"};
const char *trump_str[] = {"♣", "♦", "♥", "♠", "NT"};
/* 2 characters so Spade and South can be translated separately */
const char *trump_str_asc[] = {"Cl", "Di", "He", "Sp", "NT"};
const char *trump_str_char[] = {"C", "D", "H", "S", "N"};
const char *trump_color[] = {"#006600", "#ff8000", "#e60000", "#0000cc", "black"};
const char *double_str[] = {"", "X", "XX"};

int pos_score_for = 0; /* 0 = NS, 1 = declarer, 2 = current lead */ // TODO: implement

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

int parse_bid(char **tok)
{
	if (!strncasecmp (*tok, "p", 1) || !strncasecmp (*tok, "-", 1)) { *tok += 1; return bid_pass; }
	if (!strncasecmp (*tok, "xx", 2)) { *tok += 2; return bid_xx; }
	if (!strncasecmp (*tok, "r", 1)) { *tok += 1; return bid_xx; }
	if (!strncasecmp (*tok, "x", 1) || !strncasecmp (*tok, "d", 1)) { *tok += 1; return bid_x; }
	if (strlen (*tok) < 2) return -1;

	int alert = 0;
	if (strlen (*tok) >= 3 && *tok[2] == '!') // FIXME: can probably go
		alert = bid_alert;

	int le = *tok[0] - '0';
	int su = parse_suit ((*tok)[1]);
	if (le < 1 || le > 7 || su == -1)
		return -1;
	*tok += alert ? strlen (*tok) : 2;
	return (5 * le + su) | alert;
}

char *
card_string (card c)
{
	static char s[8];
	snprintf(s, sizeof (s), "%s%s", trump_str[SUIT(c)], rank_string(RANK(c)));
	return s;
}

char *
card_string_color (card c)
{
	static char s[60];
	snprintf(s, sizeof (s), "<span foreground=\"%s\">%s</span>%s",
			trump_color[SUIT(c)], trump_str[SUIT(c)], rank_string(RANK(c)));
	return s;
}

GString *bid_string (card c, int alert)
{
	static GString *s = NULL;
	if (!s)
		s = g_string_new(NULL);

	if (c == bid_pass) {
		g_string_printf(s, "‒"); /* FIGURE DASH */
	} else if (c == bid_x) {
		g_string_printf(s, _("X"));
	} else if (c == bid_xx) {
		g_string_printf(s, _("XX"));
	} else {
		g_string_printf(s, "%d<span foreground=\"%s\">%s</span>",
				LEVEL(c), trump_color[DENOM(c)], _(trump_str[DENOM(c)]));
	}
	return s;
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

char *
hand_suit_string (board *b, seat h, suit s)
{
	static GString *str = NULL;
	if (!str)
		str = g_string_new(NULL);
	g_string_printf(str, "");

	int c;
	for (c = s * 13 + 12; c >= (int)s * 13; c--)
		if (b->dealt_cards[c] == h)
			g_string_append_printf(str, "%s", rank_string(RANK(c)));
	return str->str;
}

char *contract_string(int level, suit trumps, seat declarer, int doubled)
{
	if (level == 0)
		return _("PASS");
	static char buf[20];
	snprintf(buf, 20, "%d%s%s %s", level, _(trump_str[trumps]),
		doubled == 2 ? _("XX") : doubled == 1 ? _("X") : "",
		_(seat_str[declarer]));
	return buf;
}

char *contract_string_asc (int level, suit trumps, seat declarer, int doubled)
{
	if (level == 0)
		return _("PASS");
	static char buf[20];
	snprintf(buf, 20, "%d%s%s %s", level, _(trump_str_asc[trumps]),
		doubled == 2 ? _("XX") : doubled == 1 ? _("X") : "",
		_(seat_str[declarer]));
	return buf;
}

seat seat_mod(seat s)
{
	return ((s + 3) % 4) + 1;
}

int seat_mask (seat s, seat filter)
{
	switch (filter) {
		case seat_none: return 0;
		case seat_all: return 1;
		case north_south: return s == north || s == south;
		case east_west: return s == east || s == west;
		default: return s == filter;
	}
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
