#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

/*
 * Standard gettext macros.
 */
#ifdef ENABLE_NLS
#  include <libintl.h>
#  undef _
#  define _(String) dgettext (PACKAGE, String)
#  define Q_(String) g_strip_context ((String), gettext (String))
#  ifdef gettext_noop
#    define N_(String) gettext_noop (String)
#  else
#    define N_(String) (String)
#  endif
#else
#  define textdomain(String) (String)
#  define gettext(String) (String)
#  define dgettext(Domain,Message) (Message)
#  define dcgettext(Domain,Message,Type) (Message)
#  define bindtextdomain(Domain,Directory) (Domain)
#  define _(String) (String)
#  define Q_(String) g_strip_context ((String), (String))
#  define N_(String) (String)
#endif


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
char *hand_suit_string (board *b, seat h, suit s);
char *contract_string(int level, suit trumps, seat declarer, int doubled);
char *contract_string_asc (int level, suit trumps, seat declarer, int doubled);
char *vuln_string (board *b);
seat seat_mod(seat s);
int seat_mask (seat s, seat filter);
char *overtricks (int i);
int score (int level, suit s, int doubled, int vuln, int tricks);
char *score_string(int level, suit trumps, seat declarer, int doubled, int vuln, int tricks, seat lead);

extern int pos_score_for;

#endif
