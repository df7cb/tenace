#include <glib.h>
#include <gtk/gtk.h>

typedef enum seat_e {
	west = 1,
	north,
	east,
	south,
} seat;

typedef enum suit_e {
	NT = 5,
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
	card cards[14]; // -1 terminated list
} hand;

typedef struct board_t {
	GString *name;
	hand *hands[4];
	seat cards[52];
	GtkWidget *win;
} board;

/*
 * prototypes
 */

hand *hand_new(char *name);
void hand_free(hand *h);
board *board_new(void);
void board_free(board *b);
gchar *rank_string (rank r);
GString *card_string (card c);
GString *hand_string (hand *h);
void remove_card(hand *h, card c);
void add_card(hand *h, card c);
int give_card(board *b, seat s, card c);

/*
 * global variables
 */

extern seat new_card_seat;
