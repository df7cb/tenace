#include <gtk/gtk.h>


void
on_neu1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on___ffnen1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_speichern1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_speichern_unter1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_beenden1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_window_hand_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_radiotoolbutton_west_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_radiotoolbutton_north_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_radiotoolbutton_east_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_radiotoolbutton_south_clicked       (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbutton_card_wipe_clicked        (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbutton_card_random_clicked      (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_hand_save_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

gboolean
on_label12_button_press_event          (GtkWidget       *widget,
                                        GdkEventButton  *event,
                                        gpointer         user_data);

void
on_button_hand_save_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dealer_west1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_north1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_east1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_item3_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_declarer_west1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_declarer_north1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_declarer_east1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_declarer_south1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contract_no_trump1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contract_spades1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contract_hearts1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contract_diamonds1_activate         (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_contract_clubs1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level2_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level3_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level4_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level5_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level6_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level7_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level_doubled1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_level_redoubled1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_double_dummy1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_parscore1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_vuln_none_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_vuln_ns_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_vuln_ew_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_vuln_all_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_set_par1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_imp_table1_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_imps_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

void
on_imps_ok_clicked                     (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_imps_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_imps_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_rewind_button_clicked               (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_handbutton_par_clicked              (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_back_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_next_clicked                 (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_fast_forward_clicked         (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_dd_toggled                   (GtkToggleToolButton *toggletoolbutton,
                                        gpointer         user_data);

void
on_pos_north_south_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_pos_declarer_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_pos_current_lead_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_deal_random_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_deal_line_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_deal_clear_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_line_entry_ok_clicked               (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_window_line_entry_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_line_entry_activate                 (GtkEntry        *entry,
                                        gpointer         user_data);

void
on_level_doubled0_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dealer_west1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dealer_north1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dealer_east1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_dealer_south1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_window_play_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_cards1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_bidding1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_play1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_window_card_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_bids1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

gboolean
on_window_bidding_delete_event         (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_window_bids_delete_event            (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_played_cards1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_style_text_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_style_cards_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_options_cards_filechooser_file_activated
                                        (GtkFileChooser  *filechooser,
                                        gpointer         user_data);

void
on_deal_new_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ddno_one1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ddeastwest1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ddnorthsouth1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ddall1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_handseastwest1_activate             (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_handsnorthsouth1_activate           (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_handsall1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_autonone1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_autoeastwest1_activate              (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_autonorthsouth1_activate            (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_autoall1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ew1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ns1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_nw1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_ne1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_sw1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_se1_activate                        (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_bid_clear_clicked                   (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_bid_undo_clicked                    (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_hand_open_clicked            (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_append1_activate                    (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_button_prev_board_clicked           (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_button_next_board_clicked           (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_info1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_aboutdialog1_close                  (GtkDialog       *dialog,
                                        gpointer         user_data);

void
on_aboutdialog1_response               (GtkDialog       *dialog,
                                        gint             response_id,
                                        gpointer         user_data);

gboolean
on_aboutdialog1_delete_event           (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

void
on_deal_cut_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_deal_copy_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data);

void
on_deal_paste_activate                 (GtkMenuItem     *menuitem,
                                        gpointer         user_data);
