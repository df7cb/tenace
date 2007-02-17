#include <ctype.h>
#include <stdio.h>

#include "bridge.h"
#include "support.h"

board *b; /* currently visible board */

static GtkWidget *card_label[52];
static GtkWidget *card_label_child[52];
static GtkWidget *card_label_container[52]; /* non-NULL if button is shown */

static GtkStatusbar *statusbar;
static guint statusbar_id = 0;

void show_board (board *b)
{
	GtkWidget *win = b->win;
	GtkWidget *w;
	GString *str;

	w = lookup_widget(win, "label_board");
	str = g_string_new(b->name->str);
	g_string_append_printf(str, "\n%s",
		contract_string(b->level, b->trumps, b->declarer, b->doubled));
	gtk_label_set_text((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_west");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[1] ? "red" : "green",
		b->current_lead == west ? " weight=\"bold\"" : "",
		b->hands[0]->name->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_north");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[0] ? "red" : "green",
		b->current_lead == north ? " weight=\"bold\"" : "",
		b->hands[1]->name->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_east");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[1] ? "red" : "green",
		b->current_lead == east ? " weight=\"bold\"" : "",
		b->hands[2]->name->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);
	w = lookup_widget(win, "label_south");
	g_string_printf(str, "<span background=\"%s\"%s>%s</span>",
		b->vuln[0] ? "red" : "green",
		b->current_lead == south ? " weight=\"bold\"" : "",
		b->hands[3]->name->str);
	gtk_label_set_markup((GtkLabel*) w, str->str);

	w = lookup_widget(win, "label_tricks");
	g_string_printf(str, "NS: %d\nEW: %d", b->tricks[0], b->tricks[1]);
	gtk_label_set_markup((GtkLabel*) w, str->str);

	/*
	int label_i;
	for (label_i = 0; label_i < 52 && b->card_label[label_i]; label_i++) {
		gtk_widget_destroy(b->card_label[label_i]);
		b->card_label[label_i] = 0;
	}
	label_i = 0;
	*/

	int c;
	GtkWidget *box;

	char *box_array[4][4] = {{"hbox_west_c", "hbox_west_d", "hbox_west_h", "hbox_west_s"},
		{"hbox_north_c", "hbox_north_d", "hbox_north_h", "hbox_north_s"},
		{"hbox_east_c", "hbox_east_d", "hbox_east_h", "hbox_east_s"},
		{"hbox_south_c", "hbox_south_d", "hbox_south_h", "hbox_south_s"}};

	for (c = 51; c >= 0; c--) {
		int h = b->cards[c];
		int s = SUIT(c);

		GtkWidget *lab = card_label[c];
		if (card_label_container[c]) {
			gtk_container_remove(GTK_CONTAINER(card_label_container[c]), lab);
			card_label_container[c] = NULL;
		}
		if (h) {
			box = lookup_widget(win, box_array[h-1][s]);
			gtk_box_pack_start (GTK_BOX (box), lab, FALSE, FALSE, FALSE);
			gtk_widget_show(lab);
			card_label_container[c] = box;
		}
	}
	gtk_widget_show_all(win);
}

void label_set_markup(card c, char *text)
{
	//gtk_label_set_markup(GTK_LABEL(card_label[c]), text);
	gtk_label_set_markup(GTK_LABEL(card_label_child[c]), text);
}

void label_clear_markups()
{
	int c;
	for (c = 0; c < 52; c++) {
		if (card_label_container[c]) {
			label_set_markup(c, rank_string(RANK(c)));
		}
	}
}

static void label_clicked(GtkLabel *l, void *foo, card *cp)
{
	printf("Clicked: %s.\n", card_string(*cp)->str);
	//gtk_label_set_markup(l, "<span background=\"red\">J</span>");
	if (play_card(b, b->cards[*cp], *cp))
		show_board(b);
}

static void label_entered(GtkLabel *l, card *cp)
{
	char buf[100];

	if (b->card_score[*cp] < 0)
		return;

	//GtkStatusbar *statusbar;
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	//static guint id = 0;
	if (!statusbar_id)
		statusbar_id = gtk_statusbar_get_context_id(statusbar, "mouseover");

	snprintf(buf, 99, "%s: %s %s",
		card_string(*cp)->str,
		contract_string(b->level, b->trumps, b->declarer, b->doubled),
		overtricks(7 - b->level - b->card_score[*cp]));
	gtk_statusbar_push(statusbar, statusbar_id, buf);
}

static void label_left(GtkLabel *l, card *cp)
{
	statusbar = GTK_STATUSBAR(lookup_widget(b->win, "statusbar1"));
	if (!statusbar_id)
		statusbar_id = gtk_statusbar_get_context_id(statusbar, "mouseover");

	gtk_statusbar_pop(statusbar, statusbar_id);
}

void create_card_labels ()
{
	static card cards[52];
	card c;
	for (c = 0; c < 52; c++) {
		//GtkWidget *wn = gtk_label_new(rank_string(RANK(c)));
		GtkWidget *lab = gtk_label_new(rank_string(RANK(c)));
		gtk_label_set_use_markup(GTK_LABEL(lab), TRUE);
		GtkWidget *wn = gtk_button_new();
		gtk_container_add(GTK_CONTAINER(wn), lab);
		//gtk_label_set_selectable (GTK_LABEL (wn), TRUE);
		gtk_container_set_border_width(GTK_CONTAINER(wn), 0);
		gtk_button_set_focus_on_click(GTK_BUTTON(wn), FALSE);
		cards[c] = c;
		g_signal_connect (wn, "button-press-event", G_CALLBACK(label_clicked), &cards[c]);
		g_signal_connect (wn, "enter", G_CALLBACK(label_entered), &cards[c]);
		g_signal_connect (wn, "leave", G_CALLBACK(label_left), &cards[c]);
		card_label[c] = wn;
		card_label_child[c] = lab;
		card_label_container[c] = NULL;
		g_object_ref(wn); // create reference so labels are not deleted when moved around
	}
}

static GString *board_format_line(board *b, char handsep, char suitsep)
{
	int h;
	GString *out = g_string_new(NULL);

	for (h = 0; h < 4; h++) {
		int s;
		card *c = b->hands[h]->cards;
		for (s = spade; s >= club; s--) {
			while (*c >= 0 && SUIT(*c) == s) {
				g_string_append_printf(out, "%c", rank_char(RANK(*c++)));
			}
			if (s > club)
				g_string_append_printf(out, "%c", suitsep);
		}
		if (h < 3)
			g_string_append_printf(out, "%c", handsep);
	}
	return out;
}

void board_save(board *b)
{
	int h;
	printf("%s\n", b->name->str);

	for (h = 0; h < 4; h++) {
		printf("%s\n", b->hands[h]->name->str);
		printf("%s\n", hand_string(b->hands[h])->str);
	}
}

static int board_parse_line(char *line, board *b, char handsep, char suitsep)
{
	seat se = west;
	char *c = line;
	int su = spade;
	while (*c && *c != '\n') {
		rank ra;
		if (*c == suitsep) {
			su--;
			if (su < 0)
				return 0;
		} else if (*c == handsep) {
			se++;
			su = spade;
			if (se > south)
				return 0;
		} else if ((ra = parse_rank_char(*c)) >= 0) {
			if (give_card(b, se, (su * 13) + ra) != 1)
				printf("could not give card?\n");
		} else {
			printf ("parse error at char %ld: %s\n", c - line + 1, line);
			return 0;
		}
		c++;
	}
	return 1;
}

int board_load(char *fname, board *b)
{
	FILE *f;
	char buf[100];
	if (!(f = fopen(fname, "r"))) {
		perror(fname);
		return 0;
	}
	if (fgets(buf, 99, f) == NULL)
		return 0;
	fclose(f);
	//printf("board is %s\n", buf);
	board_clear(b);
	if (!board_parse_line(buf, b, ' ', '.'))
		return 0;
	return 1;
}


