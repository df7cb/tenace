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
