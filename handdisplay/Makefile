CFLAGS += -O0 -g -Wall $(shell pkg-config --cflags gtk+-2.0)
LDFLAGS += $(shell pkg-config --libs gtk+-2.0)

test: hand_display.o test.o

hand_display.o: hand_display.c hand_display.h

test.o: test.c

clean:
	rm -f *.o test
