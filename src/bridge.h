#ifndef BRIDGE_H
#define BRIDGE_H

#include <glib.h>
#include <gtk/gtk.h>

typedef enum seat_e {
	west = 1,
	north,
	east,
	south,
} seat;

/* SA = 51, C2 = 0 */
typedef enum suit_e {
	NT = 4,
	spade = 3,
	heart = 2,
	diamond = 1,
	club = 0,
} suit;

typedef enum rank_e {
	card2 = 0,
	card3,
	card4,
	card5,
	card6,
	card7,
	card8,
	card9,
	card10,
	cardJ,
	cardQ,
	cardK,
	cardA,
	cardX = 0x80,
} rank;

typedef int card;

#define SUIT(c) ((int)((int)(c) / 13))
#define RANK(c) ((c) % 13)

typedef struct hand_t {
	GString *name;
	card cards[14]; // -1 terminated list, SA..S2..CA..C2
	int ncards;
} hand;

typedef struct board_t {
	GString *name;
	seat declarer;
	suit trumps;
	int level;
	int doubled;
	int vuln[2]; /* 0 = NS, 1 = EW */

	hand *hands[4];
	seat cards[52]; // 0 = not dealt
	int n_dealt_cards;
	seat dealt_cards[52]; // 0 = not dealt
	int hand_cards[4];

	int n_played_cards;
	card played_cards[52];
	seat played_cards_seat[52];
	seat current_turn;
	int tricks[2]; /* 0 = NS, 1 = EW */

	int card_score[52];
	int target[2]; /* sum might be less than 13 for partial deals */

	int par_score; /* -1 = other par_ fields invalid */
	int par_dec, par_suit, par_level, par_tricks;
	int par_arr[4][5];

	GtkWidget *win; // window showing this board
	//GtkWidget *card_label[52]; // clickable card labels
} board;

/*
 * prototypes
 */

hand *hand_new(char *name);
void hand_free(hand *h);
void board_rewind(board *b);
void calculate_target(board *b);
board *board_new(void);
void board_clear(board *b);
void board_free(board *b);
void remove_card(hand *h, card c);
//void add_card(hand *h, card c);
int give_card(board *b, seat s, card c);
int play_card(board *b, seat s, card c);

#endif
