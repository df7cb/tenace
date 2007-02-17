#ifndef FUNCTIONS_H
#define FUNCTIONS_H

char *rank_string (rank r);
char rank_char (rank r);
rank parse_rank_char (char c);
GString *card_string (card c);
char *seat_string (seat s);
GString *hand_string (board *b, seat h);
//GString *gib_string (hand *h);
char *contract_string(int level, suit trumps, seat declarer, int doubled);
seat seat_mod(seat s);
char *overtricks (int i);
int score (int level, suit s, int doubled, int vuln, int tricks);

#endif
