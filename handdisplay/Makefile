CFLAGS += -O0 -g -Wall $(shell pkg-config --cflags gtk+-2.0)
LDFLAGS += $(shell pkg-config --libs gtk+-2.0)

all: hand_display.o

hand_display.o: hand_display.c hand_display.h

install:

clean:
	rm -f *.o tags
