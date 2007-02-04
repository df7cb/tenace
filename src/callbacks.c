#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdlib.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "bridge.h"
#include "board.h"
#include "solve.h"
#include "main.h"

extern board *b;
extern GtkWidget *card_button[52];

void
on_neu1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	board_clear(b);
	show_board(b);
}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	if (!board_load("dd", b))
		printf ("open failed.\n");
	show_board(b);
}

void
on_speichern1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


void
on_speichern_unter1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_beenden1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	exit(0);
}


void
on_ausschneiden1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_kopieren1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_einf__gen1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_l__schen1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_info1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


gboolean
on_window_hand_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
	exit(1);
}


void
on_radiotoolbutton_west_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	new_card_seat = west;
}


void
on_radiotoolbutton_north_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	new_card_seat = north;
}


void
on_radiotoolbutton_east_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	new_card_seat = east;
}


void
on_radiotoolbutton_south_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	new_card_seat = south;
}


void
on_toolbutton_card_wipe_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	int c;
	for (c = 0; c < 52; c++) {
		if (b->cards[c]) {
			remove_card(b->hands[b->cards[c] - 1], c);
			b->cards[c] = 0;
			gtk_button_set_relief ((GtkButton*)card_button[c], GTK_RELIEF_NORMAL);
		}
	}
	show_board(b);
}


void
on_toolbutton_card_random_clicked      (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	int c = 0;
	while (c < 52) {
		if (b->cards[c] == 0) {
			seat s = (rand() % 4) + 1;
			if (give_card(b, s, c))
				gtk_button_set_relief ((GtkButton*)card_button[c], GTK_RELIEF_NONE);
		}
		if (b->cards[c] != 0)
			c++;
	}
	show_board(b);
}


void
on_button_hand_save_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board_save(b);
}


void
on_handbutton_gib_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data)
{
	board_dds(b);
	solve_board(b);
}

void
on_declarer_west1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->declarer = west;
	board_reset(b);
	show_board(b);
}


void
on_declarer_north1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->declarer = north;
	board_reset(b);
	show_board(b);
}


void
on_declarer_east1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->declarer = east;
	board_reset(b);
	show_board(b);
}


void
on_declarer_south1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->declarer = south;
	board_reset(b);
	show_board(b);
}


void
on_contract_no_trump1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->trumps = NT;
	show_board(b);
}


void
on_contract_spades1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->trumps = spade;
	show_board(b);
}


void
on_contract_hearts1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->trumps = heart;
	show_board(b);
}


void
on_contract_diamonds1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->trumps = diamond;
	show_board(b);
}


void
on_contract_clubs1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->trumps = club;
	show_board(b);
}


void
on_level1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 1;
	calculate_target(b);
	show_board(b);
}


void
on_level2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 2;
	calculate_target(b);
	show_board(b);
}


void
on_level3_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 3;
	calculate_target(b);
	show_board(b);
}


void
on_level4_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 4;
	calculate_target(b);
	show_board(b);
}


void
on_level5_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 5;
	calculate_target(b);
	show_board(b);
}


void
on_level6_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 6;
	calculate_target(b);
	show_board(b);
}


void
on_level7_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	b->level = 7;
	calculate_target(b);
	show_board(b);
}


void
on_level_doubled1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *check = lookup_widget(b->win, "redouble1");
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check), FALSE);
	b->doubled = 1;
	show_board(b);
}


void
on_level_redoubled1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *check = lookup_widget(b->win, "double1");
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(check), FALSE);
	b->doubled = 2;
	show_board(b);
}

void
on_board1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
}


