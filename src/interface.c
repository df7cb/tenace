/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window_hand (void)
{
  GtkWidget *window_hand;
  GtkWidget *vbox1;
  GtkWidget *menubar1;
  GtkWidget *menuitem1;
  GtkWidget *menu1;
  GtkWidget *neu1;
  GtkWidget *__ffnen1;
  GtkWidget *speichern1;
  GtkWidget *speichern_unter1;
  GtkWidget *separatormenuitem1;
  GtkWidget *beenden1;
  GtkWidget *menuitem2;
  GtkWidget *menu2;
  GtkWidget *ausschneiden1;
  GtkWidget *kopieren1;
  GtkWidget *einf__gen1;
  GtkWidget *l__schen1;
  GtkWidget *menuitem3;
  GtkWidget *menu3;
  GtkWidget *menuitem4;
  GtkWidget *menu4;
  GtkWidget *info1;
  GtkWidget *toolbar1;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *toolbutton1;
  GtkWidget *button_hand_save;
  GtkWidget *separatortoolitem2;
  GtkWidget *tmp_image;
  GtkWidget *toolbutton6;
  GtkWidget *toolbutton7;
  GtkWidget *toolbutton8;
  GtkWidget *toolbutton9;
  GtkWidget *handbutton_gib;
  GtkWidget *table1;
  GtkWidget *frame1;
  GtkWidget *alignment4;
  GtkWidget *text_west;
  GtkWidget *label_west;
  GtkWidget *frame2;
  GtkWidget *alignment5;
  GtkWidget *text_east;
  GtkWidget *label_east;
  GtkWidget *frame3;
  GtkWidget *alignment6;
  GtkWidget *table_north;
  GtkWidget *label8;
  GtkWidget *label9;
  GtkWidget *label10;
  GtkWidget *label11;
  GtkWidget *hbox_north_s;
  GtkWidget *hbox_north_h;
  GtkWidget *hbox_north_d;
  GtkWidget *hbox_north_c;
  GtkWidget *label_north;
  GtkWidget *frame4;
  GtkWidget *alignment7;
  GtkWidget *text_south;
  GtkWidget *label_south;
  GtkWidget *label_tricks;
  GtkWidget *label_board;
  GtkWidget *table4;
  GtkWidget *card_south;
  GtkWidget *card_west;
  GtkWidget *card_east;
  GtkWidget *card_north;
  GtkWidget *statusbar1;
  GtkAccelGroup *accel_group;

  accel_group = gtk_accel_group_new ();

  window_hand = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window_hand), _("bridgehand"));

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (window_hand), vbox1);

  menubar1 = gtk_menu_bar_new ();
  gtk_widget_show (menubar1);
  gtk_box_pack_start (GTK_BOX (vbox1), menubar1, FALSE, FALSE, 0);

  menuitem1 = gtk_menu_item_new_with_mnemonic (_("_Datei"));
  gtk_widget_show (menuitem1);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem1);

  menu1 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem1), menu1);

  neu1 = gtk_image_menu_item_new_from_stock ("gtk-new", accel_group);
  gtk_widget_show (neu1);
  gtk_container_add (GTK_CONTAINER (menu1), neu1);

  __ffnen1 = gtk_image_menu_item_new_from_stock ("gtk-open", accel_group);
  gtk_widget_show (__ffnen1);
  gtk_container_add (GTK_CONTAINER (menu1), __ffnen1);

  speichern1 = gtk_image_menu_item_new_from_stock ("gtk-save", accel_group);
  gtk_widget_show (speichern1);
  gtk_container_add (GTK_CONTAINER (menu1), speichern1);

  speichern_unter1 = gtk_image_menu_item_new_from_stock ("gtk-save-as", accel_group);
  gtk_widget_show (speichern_unter1);
  gtk_container_add (GTK_CONTAINER (menu1), speichern_unter1);

  separatormenuitem1 = gtk_separator_menu_item_new ();
  gtk_widget_show (separatormenuitem1);
  gtk_container_add (GTK_CONTAINER (menu1), separatormenuitem1);
  gtk_widget_set_sensitive (separatormenuitem1, FALSE);

  beenden1 = gtk_image_menu_item_new_from_stock ("gtk-quit", accel_group);
  gtk_widget_show (beenden1);
  gtk_container_add (GTK_CONTAINER (menu1), beenden1);

  menuitem2 = gtk_menu_item_new_with_mnemonic (_("_Bearbeiten"));
  gtk_widget_show (menuitem2);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem2);

  menu2 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem2), menu2);

  ausschneiden1 = gtk_image_menu_item_new_from_stock ("gtk-cut", accel_group);
  gtk_widget_show (ausschneiden1);
  gtk_container_add (GTK_CONTAINER (menu2), ausschneiden1);

  kopieren1 = gtk_image_menu_item_new_from_stock ("gtk-copy", accel_group);
  gtk_widget_show (kopieren1);
  gtk_container_add (GTK_CONTAINER (menu2), kopieren1);

  einf__gen1 = gtk_image_menu_item_new_from_stock ("gtk-paste", accel_group);
  gtk_widget_show (einf__gen1);
  gtk_container_add (GTK_CONTAINER (menu2), einf__gen1);

  l__schen1 = gtk_image_menu_item_new_from_stock ("gtk-delete", accel_group);
  gtk_widget_show (l__schen1);
  gtk_container_add (GTK_CONTAINER (menu2), l__schen1);

  menuitem3 = gtk_menu_item_new_with_mnemonic (_("_Ansicht"));
  gtk_widget_show (menuitem3);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem3);

  menu3 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem3), menu3);

  menuitem4 = gtk_menu_item_new_with_mnemonic (_("_Hilfe"));
  gtk_widget_show (menuitem4);
  gtk_container_add (GTK_CONTAINER (menubar1), menuitem4);

  menu4 = gtk_menu_new ();
  gtk_menu_item_set_submenu (GTK_MENU_ITEM (menuitem4), menu4);

  info1 = gtk_menu_item_new_with_mnemonic (_("_Info"));
  gtk_widget_show (info1);
  gtk_container_add (GTK_CONTAINER (menu4), info1);

  toolbar1 = gtk_toolbar_new ();
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar1), GTK_TOOLBAR_BOTH_HORIZ);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar1));

  toolbutton1 = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-open");
  gtk_widget_show (toolbutton1);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbutton1);

  button_hand_save = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-save");
  gtk_widget_show (button_hand_save);
  gtk_container_add (GTK_CONTAINER (toolbar1), button_hand_save);

  separatortoolitem2 = (GtkWidget*) gtk_separator_tool_item_new ();
  gtk_widget_show (separatortoolitem2);
  gtk_container_add (GTK_CONTAINER (toolbar1), separatortoolitem2);

  tmp_image = gtk_image_new_from_stock ("gtk-media-previous", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton6 = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Anfang"));
  gtk_widget_show (toolbutton6);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbutton6);

  tmp_image = gtk_image_new_from_stock ("gtk-media-rewind", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton7 = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Zur\303\274ck"));
  gtk_widget_show (toolbutton7);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbutton7);

  tmp_image = gtk_image_new_from_stock ("gtk-media-forward", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton8 = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Vor"));
  gtk_widget_show (toolbutton8);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbutton8);

  tmp_image = gtk_image_new_from_stock ("gtk-media-next", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton9 = (GtkWidget*) gtk_tool_button_new (tmp_image, "");
  gtk_widget_show (toolbutton9);
  gtk_container_add (GTK_CONTAINER (toolbar1), toolbutton9);

  handbutton_gib = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-convert");
  gtk_widget_show (handbutton_gib);
  gtk_container_add (GTK_CONTAINER (toolbar1), handbutton_gib);

  table1 = gtk_table_new (3, 3, FALSE);
  gtk_widget_show (table1);
  gtk_box_pack_start (GTK_BOX (vbox1), table1, TRUE, TRUE, 0);

  frame1 = gtk_frame_new (NULL);
  gtk_widget_show (frame1);
  gtk_table_attach (GTK_TABLE (table1), frame1, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame1), 5);

  alignment4 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (frame1), alignment4);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment4), 0, 0, 12, 0);

  text_west = gtk_label_new (_("\342\231\240 AK1098\n\342\231\245 AQJ4\n\342\231\246 KJ10\n\342\231\243 2"));
  gtk_widget_show (text_west);
  gtk_container_add (GTK_CONTAINER (alignment4), text_west);
  GTK_WIDGET_SET_FLAGS (text_west, GTK_CAN_FOCUS);
  gtk_label_set_selectable (GTK_LABEL (text_west), TRUE);
  gtk_misc_set_alignment (GTK_MISC (text_west), 0, 0.5);

  label_west = gtk_label_new (_("West"));
  gtk_widget_show (label_west);
  gtk_frame_set_label_widget (GTK_FRAME (frame1), label_west);
  gtk_label_set_use_markup (GTK_LABEL (label_west), TRUE);

  frame2 = gtk_frame_new (NULL);
  gtk_widget_show (frame2);
  gtk_table_attach (GTK_TABLE (table1), frame2, 2, 3, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame2), 5);

  alignment5 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment5);
  gtk_container_add (GTK_CONTAINER (frame2), alignment5);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment5), 0, 0, 12, 0);

  text_east = gtk_label_new (_("\342\231\240AK1098\n\342\231\245AQJ4\n\342\231\246KJ10\n\342\231\2432"));
  gtk_widget_show (text_east);
  gtk_container_add (GTK_CONTAINER (alignment5), text_east);
  GTK_WIDGET_SET_FLAGS (text_east, GTK_CAN_FOCUS);
  gtk_label_set_selectable (GTK_LABEL (text_east), TRUE);
  gtk_misc_set_alignment (GTK_MISC (text_east), 0, 0.5);

  label_east = gtk_label_new (_("Ost"));
  gtk_widget_show (label_east);
  gtk_frame_set_label_widget (GTK_FRAME (frame2), label_east);
  gtk_label_set_use_markup (GTK_LABEL (label_east), TRUE);

  frame3 = gtk_frame_new (NULL);
  gtk_widget_show (frame3);
  gtk_table_attach (GTK_TABLE (table1), frame3, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame3), 5);

  alignment6 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment6);
  gtk_container_add (GTK_CONTAINER (frame3), alignment6);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment6), 0, 0, 12, 0);

  table_north = gtk_table_new (4, 2, FALSE);
  gtk_widget_show (table_north);
  gtk_container_add (GTK_CONTAINER (alignment6), table_north);

  label8 = gtk_label_new (_("\342\231\240"));
  gtk_widget_show (label8);
  gtk_table_attach (GTK_TABLE (table_north), label8, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_padding (GTK_MISC (label8), 5, 0);

  label9 = gtk_label_new (_("\342\231\245"));
  gtk_widget_show (label9);
  gtk_table_attach (GTK_TABLE (table_north), label9, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label10 = gtk_label_new (_("\342\231\246"));
  gtk_widget_show (label10);
  gtk_table_attach (GTK_TABLE (table_north), label10, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label11 = gtk_label_new (_("\342\231\243"));
  gtk_widget_show (label11);
  gtk_table_attach (GTK_TABLE (table_north), label11, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbox_north_s = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_north_s);
  gtk_table_attach (GTK_TABLE (table_north), hbox_north_s, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hbox_north_h = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_north_h);
  gtk_table_attach (GTK_TABLE (table_north), hbox_north_h, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hbox_north_d = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_north_d);
  gtk_table_attach (GTK_TABLE (table_north), hbox_north_d, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  hbox_north_c = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (hbox_north_c);
  gtk_table_attach (GTK_TABLE (table_north), hbox_north_c, 1, 2, 3, 4,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  label_north = gtk_label_new (_("Nord"));
  gtk_widget_show (label_north);
  gtk_frame_set_label_widget (GTK_FRAME (frame3), label_north);
  gtk_label_set_use_markup (GTK_LABEL (label_north), TRUE);

  frame4 = gtk_frame_new (NULL);
  gtk_widget_show (frame4);
  gtk_table_attach (GTK_TABLE (table1), frame4, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame4), 5);

  alignment7 = gtk_alignment_new (0.5, 0.5, 1, 1);
  gtk_widget_show (alignment7);
  gtk_container_add (GTK_CONTAINER (frame4), alignment7);
  gtk_alignment_set_padding (GTK_ALIGNMENT (alignment7), 0, 0, 12, 0);

  text_south = gtk_label_new (_("\342\231\240\n\342\231\2459\n\342\231\246\n\342\231\243"));
  gtk_widget_show (text_south);
  gtk_container_add (GTK_CONTAINER (alignment7), text_south);
  GTK_WIDGET_SET_FLAGS (text_south, GTK_CAN_FOCUS);
  gtk_label_set_selectable (GTK_LABEL (text_south), TRUE);
  gtk_misc_set_alignment (GTK_MISC (text_south), 0, 0.5);

  label_south = gtk_label_new (_("S\303\274d"));
  gtk_widget_show (label_south);
  gtk_frame_set_label_widget (GTK_FRAME (frame4), label_south);
  gtk_label_set_use_markup (GTK_LABEL (label_south), TRUE);

  label_tricks = gtk_label_new (_("NS: 0\nOW: 0"));
  gtk_widget_show (label_tricks);
  gtk_table_attach (GTK_TABLE (table1), label_tricks, 2, 3, 2, 3,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 5, 0);
  GTK_WIDGET_SET_FLAGS (label_tricks, GTK_CAN_FOCUS);
  gtk_label_set_justify (GTK_LABEL (label_tricks), GTK_JUSTIFY_RIGHT);
  gtk_label_set_selectable (GTK_LABEL (label_tricks), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label_tricks), 1, 1);

  label_board = gtk_label_new (_("Board 1\nD: W, V: NS\n4\342\231\246X S"));
  gtk_widget_show (label_board);
  gtk_table_attach (GTK_TABLE (table1), label_board, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 5, 0);
  GTK_WIDGET_SET_FLAGS (label_board, GTK_CAN_FOCUS);
  gtk_label_set_selectable (GTK_LABEL (label_board), TRUE);
  gtk_misc_set_alignment (GTK_MISC (label_board), 0, 0);

  table4 = gtk_table_new (3, 2, FALSE);
  gtk_widget_show (table4);
  gtk_table_attach (GTK_TABLE (table1), table4, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (GTK_FILL), 0, 0);

  card_south = gtk_label_new (_("\342\231\2459"));
  gtk_widget_show (card_south);
  gtk_table_attach (GTK_TABLE (table4), card_south, 0, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);
  GTK_WIDGET_SET_FLAGS (card_south, GTK_CAN_FOCUS);
  gtk_label_set_justify (GTK_LABEL (card_south), GTK_JUSTIFY_CENTER);
  gtk_label_set_selectable (GTK_LABEL (card_south), TRUE);

  card_west = gtk_label_new (_("\342\231\245A"));
  gtk_widget_show (card_west);
  gtk_table_attach (GTK_TABLE (table4), card_west, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);
  GTK_WIDGET_SET_FLAGS (card_west, GTK_CAN_FOCUS);
  gtk_label_set_justify (GTK_LABEL (card_west), GTK_JUSTIFY_CENTER);
  gtk_label_set_selectable (GTK_LABEL (card_west), TRUE);
  gtk_misc_set_alignment (GTK_MISC (card_west), 0.49, 0.5);

  card_east = gtk_label_new (_("\342\231\2454"));
  gtk_widget_show (card_east);
  gtk_table_attach (GTK_TABLE (table4), card_east, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);
  GTK_WIDGET_SET_FLAGS (card_east, GTK_CAN_FOCUS);
  gtk_label_set_justify (GTK_LABEL (card_east), GTK_JUSTIFY_CENTER);
  gtk_label_set_selectable (GTK_LABEL (card_east), TRUE);

  card_north = gtk_label_new (_("\342\231\2452"));
  gtk_widget_show (card_north);
  gtk_table_attach (GTK_TABLE (table4), card_north, 0, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND), 0, 0);
  GTK_WIDGET_SET_FLAGS (card_north, GTK_CAN_FOCUS);
  gtk_label_set_justify (GTK_LABEL (card_north), GTK_JUSTIFY_CENTER);
  gtk_label_set_selectable (GTK_LABEL (card_north), TRUE);

  statusbar1 = gtk_statusbar_new ();
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox1), statusbar1, FALSE, FALSE, 0);

  g_signal_connect ((gpointer) window_hand, "delete_event",
                    G_CALLBACK (on_window_hand_delete_event),
                    NULL);
  g_signal_connect ((gpointer) neu1, "activate",
                    G_CALLBACK (on_neu1_activate),
                    NULL);
  g_signal_connect ((gpointer) __ffnen1, "activate",
                    G_CALLBACK (on___ffnen1_activate),
                    NULL);
  g_signal_connect ((gpointer) speichern1, "activate",
                    G_CALLBACK (on_speichern1_activate),
                    NULL);
  g_signal_connect ((gpointer) speichern_unter1, "activate",
                    G_CALLBACK (on_speichern_unter1_activate),
                    NULL);
  g_signal_connect ((gpointer) beenden1, "activate",
                    G_CALLBACK (on_beenden1_activate),
                    NULL);
  g_signal_connect ((gpointer) ausschneiden1, "activate",
                    G_CALLBACK (on_ausschneiden1_activate),
                    NULL);
  g_signal_connect ((gpointer) kopieren1, "activate",
                    G_CALLBACK (on_kopieren1_activate),
                    NULL);
  g_signal_connect ((gpointer) einf__gen1, "activate",
                    G_CALLBACK (on_einf__gen1_activate),
                    NULL);
  g_signal_connect ((gpointer) l__schen1, "activate",
                    G_CALLBACK (on_l__schen1_activate),
                    NULL);
  g_signal_connect ((gpointer) info1, "activate",
                    G_CALLBACK (on_info1_activate),
                    NULL);
  g_signal_connect ((gpointer) button_hand_save, "clicked",
                    G_CALLBACK (on_button_hand_save_clicked),
                    NULL);
  g_signal_connect ((gpointer) handbutton_gib, "clicked",
                    G_CALLBACK (on_handbutton_gib_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_hand, window_hand, "window_hand");
  GLADE_HOOKUP_OBJECT (window_hand, vbox1, "vbox1");
  GLADE_HOOKUP_OBJECT (window_hand, menubar1, "menubar1");
  GLADE_HOOKUP_OBJECT (window_hand, menuitem1, "menuitem1");
  GLADE_HOOKUP_OBJECT (window_hand, menu1, "menu1");
  GLADE_HOOKUP_OBJECT (window_hand, neu1, "neu1");
  GLADE_HOOKUP_OBJECT (window_hand, __ffnen1, "__ffnen1");
  GLADE_HOOKUP_OBJECT (window_hand, speichern1, "speichern1");
  GLADE_HOOKUP_OBJECT (window_hand, speichern_unter1, "speichern_unter1");
  GLADE_HOOKUP_OBJECT (window_hand, separatormenuitem1, "separatormenuitem1");
  GLADE_HOOKUP_OBJECT (window_hand, beenden1, "beenden1");
  GLADE_HOOKUP_OBJECT (window_hand, menuitem2, "menuitem2");
  GLADE_HOOKUP_OBJECT (window_hand, menu2, "menu2");
  GLADE_HOOKUP_OBJECT (window_hand, ausschneiden1, "ausschneiden1");
  GLADE_HOOKUP_OBJECT (window_hand, kopieren1, "kopieren1");
  GLADE_HOOKUP_OBJECT (window_hand, einf__gen1, "einf__gen1");
  GLADE_HOOKUP_OBJECT (window_hand, l__schen1, "l__schen1");
  GLADE_HOOKUP_OBJECT (window_hand, menuitem3, "menuitem3");
  GLADE_HOOKUP_OBJECT (window_hand, menu3, "menu3");
  GLADE_HOOKUP_OBJECT (window_hand, menuitem4, "menuitem4");
  GLADE_HOOKUP_OBJECT (window_hand, menu4, "menu4");
  GLADE_HOOKUP_OBJECT (window_hand, info1, "info1");
  GLADE_HOOKUP_OBJECT (window_hand, toolbar1, "toolbar1");
  GLADE_HOOKUP_OBJECT (window_hand, toolbutton1, "toolbutton1");
  GLADE_HOOKUP_OBJECT (window_hand, button_hand_save, "button_hand_save");
  GLADE_HOOKUP_OBJECT (window_hand, separatortoolitem2, "separatortoolitem2");
  GLADE_HOOKUP_OBJECT (window_hand, toolbutton6, "toolbutton6");
  GLADE_HOOKUP_OBJECT (window_hand, toolbutton7, "toolbutton7");
  GLADE_HOOKUP_OBJECT (window_hand, toolbutton8, "toolbutton8");
  GLADE_HOOKUP_OBJECT (window_hand, toolbutton9, "toolbutton9");
  GLADE_HOOKUP_OBJECT (window_hand, handbutton_gib, "handbutton_gib");
  GLADE_HOOKUP_OBJECT (window_hand, table1, "table1");
  GLADE_HOOKUP_OBJECT (window_hand, frame1, "frame1");
  GLADE_HOOKUP_OBJECT (window_hand, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (window_hand, text_west, "text_west");
  GLADE_HOOKUP_OBJECT (window_hand, label_west, "label_west");
  GLADE_HOOKUP_OBJECT (window_hand, frame2, "frame2");
  GLADE_HOOKUP_OBJECT (window_hand, alignment5, "alignment5");
  GLADE_HOOKUP_OBJECT (window_hand, text_east, "text_east");
  GLADE_HOOKUP_OBJECT (window_hand, label_east, "label_east");
  GLADE_HOOKUP_OBJECT (window_hand, frame3, "frame3");
  GLADE_HOOKUP_OBJECT (window_hand, alignment6, "alignment6");
  GLADE_HOOKUP_OBJECT (window_hand, table_north, "table_north");
  GLADE_HOOKUP_OBJECT (window_hand, label8, "label8");
  GLADE_HOOKUP_OBJECT (window_hand, label9, "label9");
  GLADE_HOOKUP_OBJECT (window_hand, label10, "label10");
  GLADE_HOOKUP_OBJECT (window_hand, label11, "label11");
  GLADE_HOOKUP_OBJECT (window_hand, hbox_north_s, "hbox_north_s");
  GLADE_HOOKUP_OBJECT (window_hand, hbox_north_h, "hbox_north_h");
  GLADE_HOOKUP_OBJECT (window_hand, hbox_north_d, "hbox_north_d");
  GLADE_HOOKUP_OBJECT (window_hand, hbox_north_c, "hbox_north_c");
  GLADE_HOOKUP_OBJECT (window_hand, label_north, "label_north");
  GLADE_HOOKUP_OBJECT (window_hand, frame4, "frame4");
  GLADE_HOOKUP_OBJECT (window_hand, alignment7, "alignment7");
  GLADE_HOOKUP_OBJECT (window_hand, text_south, "text_south");
  GLADE_HOOKUP_OBJECT (window_hand, label_south, "label_south");
  GLADE_HOOKUP_OBJECT (window_hand, label_tricks, "label_tricks");
  GLADE_HOOKUP_OBJECT (window_hand, label_board, "label_board");
  GLADE_HOOKUP_OBJECT (window_hand, table4, "table4");
  GLADE_HOOKUP_OBJECT (window_hand, card_south, "card_south");
  GLADE_HOOKUP_OBJECT (window_hand, card_west, "card_west");
  GLADE_HOOKUP_OBJECT (window_hand, card_east, "card_east");
  GLADE_HOOKUP_OBJECT (window_hand, card_north, "card_north");
  GLADE_HOOKUP_OBJECT (window_hand, statusbar1, "statusbar1");

  gtk_window_add_accel_group (GTK_WINDOW (window_hand), accel_group);

  return window_hand;
}

GtkWidget*
create_window_bid (void)
{
  GtkWidget *window_bid;
  GtkWidget *scrolledwindow1;
  GtkWidget *treeview_bid;

  window_bid = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window_bid), _("Bidding"));

  scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow1);
  gtk_container_add (GTK_CONTAINER (window_bid), scrolledwindow1);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (scrolledwindow1), GTK_SHADOW_IN);

  treeview_bid = gtk_tree_view_new ();
  gtk_widget_show (treeview_bid);
  gtk_container_add (GTK_CONTAINER (scrolledwindow1), treeview_bid);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_bid, window_bid, "window_bid");
  GLADE_HOOKUP_OBJECT (window_bid, scrolledwindow1, "scrolledwindow1");
  GLADE_HOOKUP_OBJECT (window_bid, treeview_bid, "treeview_bid");

  return window_bid;
}

GtkWidget*
create_window_card (void)
{
  GtkWidget *window_card;
  GtkWidget *vbox2;
  GtkWidget *toolbar2;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *tmp_image;
  GtkWidget *toolbutton_card_wipe;
  GtkWidget *toolbutton_card_random;
  GtkWidget *separatortoolitem1;
  GSList *radiotoolbutton_west_group = NULL;
  GtkWidget *radiotoolbutton_west;
  GtkWidget *radiotoolbutton_north;
  GtkWidget *radiotoolbutton_east;
  GtkWidget *radiotoolbutton_south;
  GtkWidget *table_cards;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *label3;
  GtkWidget *label4;

  window_card = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window_card), _("Cards"));

  vbox2 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (window_card), vbox2);

  toolbar2 = gtk_toolbar_new ();
  gtk_widget_show (toolbar2);
  gtk_box_pack_start (GTK_BOX (vbox2), toolbar2, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar2), GTK_TOOLBAR_ICONS);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar2));

  tmp_image = gtk_image_new_from_stock ("gtk-delete", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton_card_wipe = (GtkWidget*) gtk_tool_button_new (tmp_image, "");
  gtk_widget_show (toolbutton_card_wipe);
  gtk_container_add (GTK_CONTAINER (toolbar2), toolbutton_card_wipe);

  tmp_image = gtk_image_new_from_stock ("gtk-select-color", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  toolbutton_card_random = (GtkWidget*) gtk_tool_button_new (tmp_image, _("Zufall"));
  gtk_widget_show (toolbutton_card_random);
  gtk_container_add (GTK_CONTAINER (toolbar2), toolbutton_card_random);

  separatortoolitem1 = (GtkWidget*) gtk_separator_tool_item_new ();
  gtk_widget_show (separatortoolitem1);
  gtk_container_add (GTK_CONTAINER (toolbar2), separatortoolitem1);

  radiotoolbutton_west = (GtkWidget*) gtk_radio_tool_button_new (NULL);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (radiotoolbutton_west), _("West"));
  tmp_image = gtk_image_new_from_stock ("gtk-go-back", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (radiotoolbutton_west), tmp_image);
  gtk_widget_show (radiotoolbutton_west);
  gtk_container_add (GTK_CONTAINER (toolbar2), radiotoolbutton_west);
  gtk_radio_tool_button_set_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_west), radiotoolbutton_west_group);
  radiotoolbutton_west_group = gtk_radio_tool_button_get_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_west));

  radiotoolbutton_north = (GtkWidget*) gtk_radio_tool_button_new (NULL);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (radiotoolbutton_north), _("Nord"));
  tmp_image = gtk_image_new_from_stock ("gtk-go-up", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (radiotoolbutton_north), tmp_image);
  gtk_widget_show (radiotoolbutton_north);
  gtk_container_add (GTK_CONTAINER (toolbar2), radiotoolbutton_north);
  gtk_radio_tool_button_set_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_north), radiotoolbutton_west_group);
  radiotoolbutton_west_group = gtk_radio_tool_button_get_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_north));

  radiotoolbutton_east = (GtkWidget*) gtk_radio_tool_button_new (NULL);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (radiotoolbutton_east), _("Ost"));
  tmp_image = gtk_image_new_from_stock ("gtk-go-forward", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (radiotoolbutton_east), tmp_image);
  gtk_widget_show (radiotoolbutton_east);
  gtk_container_add (GTK_CONTAINER (toolbar2), radiotoolbutton_east);
  gtk_radio_tool_button_set_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_east), radiotoolbutton_west_group);
  radiotoolbutton_west_group = gtk_radio_tool_button_get_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_east));

  radiotoolbutton_south = (GtkWidget*) gtk_radio_tool_button_new (NULL);
  gtk_tool_button_set_label (GTK_TOOL_BUTTON (radiotoolbutton_south), _("S\303\274d"));
  tmp_image = gtk_image_new_from_stock ("gtk-go-down", tmp_toolbar_icon_size);
  gtk_widget_show (tmp_image);
  gtk_tool_button_set_icon_widget (GTK_TOOL_BUTTON (radiotoolbutton_south), tmp_image);
  gtk_widget_show (radiotoolbutton_south);
  gtk_container_add (GTK_CONTAINER (toolbar2), radiotoolbutton_south);
  gtk_radio_tool_button_set_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_south), radiotoolbutton_west_group);
  radiotoolbutton_west_group = gtk_radio_tool_button_get_group (GTK_RADIO_TOOL_BUTTON (radiotoolbutton_south));

  table_cards = gtk_table_new (4, 15, FALSE);
  gtk_widget_show (table_cards);
  gtk_box_pack_start (GTK_BOX (vbox2), table_cards, FALSE, FALSE, 0);

  label1 = gtk_label_new (_("\342\231\240"));
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table_cards), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK), 0, 0);

  label2 = gtk_label_new (_("\342\231\245"));
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table_cards), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK), 0, 0);

  label3 = gtk_label_new (_("\342\231\246"));
  gtk_widget_show (label3);
  gtk_table_attach (GTK_TABLE (table_cards), label3, 0, 1, 2, 3,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK), 0, 0);

  label4 = gtk_label_new (_("\342\231\243"));
  gtk_widget_show (label4);
  gtk_table_attach (GTK_TABLE (table_cards), label4, 0, 1, 3, 4,
                    (GtkAttachOptions) (GTK_SHRINK | GTK_FILL),
                    (GtkAttachOptions) (GTK_EXPAND | GTK_SHRINK), 0, 0);

  g_signal_connect ((gpointer) toolbutton_card_wipe, "clicked",
                    G_CALLBACK (on_toolbutton_card_wipe_clicked),
                    NULL);
  g_signal_connect ((gpointer) toolbutton_card_random, "clicked",
                    G_CALLBACK (on_toolbutton_card_random_clicked),
                    NULL);
  g_signal_connect ((gpointer) radiotoolbutton_west, "clicked",
                    G_CALLBACK (on_radiotoolbutton_west_clicked),
                    NULL);
  g_signal_connect ((gpointer) radiotoolbutton_north, "clicked",
                    G_CALLBACK (on_radiotoolbutton_north_clicked),
                    NULL);
  g_signal_connect ((gpointer) radiotoolbutton_east, "clicked",
                    G_CALLBACK (on_radiotoolbutton_east_clicked),
                    NULL);
  g_signal_connect ((gpointer) radiotoolbutton_south, "clicked",
                    G_CALLBACK (on_radiotoolbutton_south_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_card, window_card, "window_card");
  GLADE_HOOKUP_OBJECT (window_card, vbox2, "vbox2");
  GLADE_HOOKUP_OBJECT (window_card, toolbar2, "toolbar2");
  GLADE_HOOKUP_OBJECT (window_card, toolbutton_card_wipe, "toolbutton_card_wipe");
  GLADE_HOOKUP_OBJECT (window_card, toolbutton_card_random, "toolbutton_card_random");
  GLADE_HOOKUP_OBJECT (window_card, separatortoolitem1, "separatortoolitem1");
  GLADE_HOOKUP_OBJECT (window_card, radiotoolbutton_west, "radiotoolbutton_west");
  GLADE_HOOKUP_OBJECT (window_card, radiotoolbutton_north, "radiotoolbutton_north");
  GLADE_HOOKUP_OBJECT (window_card, radiotoolbutton_east, "radiotoolbutton_east");
  GLADE_HOOKUP_OBJECT (window_card, radiotoolbutton_south, "radiotoolbutton_south");
  GLADE_HOOKUP_OBJECT (window_card, table_cards, "table_cards");
  GLADE_HOOKUP_OBJECT (window_card, label1, "label1");
  GLADE_HOOKUP_OBJECT (window_card, label2, "label2");
  GLADE_HOOKUP_OBJECT (window_card, label3, "label3");
  GLADE_HOOKUP_OBJECT (window_card, label4, "label4");

  return window_card;
}

GtkWidget*
create_window_bids (void)
{
  GtkWidget *window_bids;
  GtkWidget *vbox3;
  GtkWidget *toolbar3;
  GtkIconSize tmp_toolbar_icon_size;
  GtkWidget *toolbutton5;
  GtkWidget *hbuttonbox1;
  GtkWidget *button1;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image1;
  GtkWidget *label5;
  GtkWidget *button2;
  GtkWidget *alignment2;
  GtkWidget *hbox2;
  GtkWidget *image2;
  GtkWidget *label6;
  GtkWidget *button3;
  GtkWidget *alignment3;
  GtkWidget *hbox3;
  GtkWidget *image3;
  GtkWidget *label7;
  GtkWidget *table3;

  window_bids = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window_bids), _("Bids"));

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (window_bids), vbox3);

  toolbar3 = gtk_toolbar_new ();
  gtk_widget_show (toolbar3);
  gtk_box_pack_start (GTK_BOX (vbox3), toolbar3, FALSE, FALSE, 0);
  gtk_toolbar_set_style (GTK_TOOLBAR (toolbar3), GTK_TOOLBAR_ICONS);
  tmp_toolbar_icon_size = gtk_toolbar_get_icon_size (GTK_TOOLBAR (toolbar3));

  toolbutton5 = (GtkWidget*) gtk_tool_button_new_from_stock ("gtk-delete");
  gtk_widget_show (toolbutton5);
  gtk_container_add (GTK_CONTAINER (toolbar3), toolbutton5);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox3), hbuttonbox1, FALSE, TRUE, 0);

  button1 = gtk_button_new ();
  gtk_widget_show (button1);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button1);
  GTK_WIDGET_SET_FLAGS (button1, GTK_CAN_DEFAULT);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button1), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image1 = gtk_image_new_from_stock ("gtk-remove", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image1);
  gtk_box_pack_start (GTK_BOX (hbox1), image1, FALSE, FALSE, 0);

  label5 = gtk_label_new_with_mnemonic (_("Pass"));
  gtk_widget_show (label5);
  gtk_box_pack_start (GTK_BOX (hbox1), label5, FALSE, FALSE, 0);

  button2 = gtk_button_new ();
  gtk_widget_show (button2);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button2);
  GTK_WIDGET_SET_FLAGS (button2, GTK_CAN_DEFAULT);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button2), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image2 = gtk_image_new_from_stock ("gtk-stop", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox2), image2, FALSE, FALSE, 0);

  label6 = gtk_label_new_with_mnemonic (_("Kontra"));
  gtk_widget_show (label6);
  gtk_box_pack_start (GTK_BOX (hbox2), label6, FALSE, FALSE, 0);

  button3 = gtk_button_new ();
  gtk_widget_show (button3);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), button3);
  GTK_WIDGET_SET_FLAGS (button3, GTK_CAN_DEFAULT);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button3), alignment3);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox3);

  image3 = gtk_image_new_from_stock ("gtk-stop", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox3), image3, FALSE, FALSE, 0);

  label7 = gtk_label_new_with_mnemonic (_("Rekontra"));
  gtk_widget_show (label7);
  gtk_box_pack_start (GTK_BOX (hbox3), label7, FALSE, FALSE, 0);

  table3 = gtk_table_new (7, 4, FALSE);
  gtk_widget_show (table3);
  gtk_box_pack_start (GTK_BOX (vbox3), table3, TRUE, TRUE, 0);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window_bids, window_bids, "window_bids");
  GLADE_HOOKUP_OBJECT (window_bids, vbox3, "vbox3");
  GLADE_HOOKUP_OBJECT (window_bids, toolbar3, "toolbar3");
  GLADE_HOOKUP_OBJECT (window_bids, toolbutton5, "toolbutton5");
  GLADE_HOOKUP_OBJECT (window_bids, hbuttonbox1, "hbuttonbox1");
  GLADE_HOOKUP_OBJECT (window_bids, button1, "button1");
  GLADE_HOOKUP_OBJECT (window_bids, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (window_bids, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (window_bids, image1, "image1");
  GLADE_HOOKUP_OBJECT (window_bids, label5, "label5");
  GLADE_HOOKUP_OBJECT (window_bids, button2, "button2");
  GLADE_HOOKUP_OBJECT (window_bids, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (window_bids, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (window_bids, image2, "image2");
  GLADE_HOOKUP_OBJECT (window_bids, label6, "label6");
  GLADE_HOOKUP_OBJECT (window_bids, button3, "button3");
  GLADE_HOOKUP_OBJECT (window_bids, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (window_bids, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (window_bids, image3, "image3");
  GLADE_HOOKUP_OBJECT (window_bids, label7, "label7");
  GLADE_HOOKUP_OBJECT (window_bids, table3, "table3");

  return window_bids;
}

