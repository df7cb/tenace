#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "bridge.h"

extern const char *seat_str[];
extern const char *trump_str[];
extern const char *trump_str_asc[];
extern const char *trump_str_char[];
extern const char *trump_color[];
extern const char *double_str[];

char *rank_string (rank r);
char rank_char (rank r);
int parse_suit(char c);
int parse_card(char *tok);
int parse_bid(char **tok);
rank parse_rank_char (char c);
char *card_string (card c);
char *card_string_color (card c);
GString *bid_string (card c, int alert);
GString *hand_string (board *b, seat h);
char *contract_string(int level, suit trumps, seat declarer, int doubled);
char *contract_string_asc (int level, suit trumps, seat declarer, int doubled);
seat seat_mod(seat s);
int seat_mask (seat s, seat filter);
char *overtricks (int i);
int score (int level, suit s, int doubled, int vuln, int tricks);
char *score_string(int level, suit trumps, seat declarer, int doubled, int vuln, int tricks, seat lead);

extern int pos_score_for;

#endif
