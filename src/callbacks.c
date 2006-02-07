#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "bridge.h"
#include "board.h"

extern board *b;
extern GtkWidget *card_button[52];

void
on_neu1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on___ffnen1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

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

