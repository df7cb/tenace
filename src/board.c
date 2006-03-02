#include <stdio.h>

#include "bridge.h"

void board_save(board *b)
{
	int h;
	printf("%s\n", b->name->str);

	for (h = 0; h < 4; h++) {
		printf("%s\n", b->hands[h]->name->str);
		printf("%s\n", hand_string(b->hands[h])->str);
	}
}

void board_gib(board *b)
{
	char *h_char[] = {"w", "n", "e", "s"};
	int h;
	FILE *f;
	if (!(f = fopen("dd", "w"))) {
		perror("dd");
		return;
	}
	for (h = 0; h < 4; h++) {
		fprintf(f, "%s ", h_char[h]);
		fprintf(f, "%s\n", gib_string(b->hands[h])->str);
	}
	fprintf(f, "s s\nca\n");
	fclose(f);
	if (!(f = fopen("dc", "w"))) {
		perror("dc");
		return;
	}
	fprintf(f, "-vd dd\n");
	fclose(f);
	system("wine /home/cb/projects/bridge/gib/zip/bridge.exe -i dc");
}
