#include <glib.h>
#include <gtk/gtk.h>

typedef enum seat_e {
	west = 1,
	north,
	east,
	south,
} seat;

typedef enum suit_e {
	NT = 0,
	spade = 4,
	heart = 3,
	diamond = 2,
	club = 1,
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
	cardX,
} rank;

typedef int card;

#define SUIT(c) (((c) & 0x70) >> 4)
#define RANK(c) ((c) & 0x0f)
#define CARD_I(s, r) (51 - ((r) + 13 * ((s) - 1)))

typedef struct hand_t {
	GString *name;
	card cards[14]; // 0-terminated list
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

hand *hand_new(void);
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
