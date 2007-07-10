CC = @CC@
CFLAGS += @PACKAGE_CFLAGS@
LDFLAGS += @PACKAGE_LIBS@

EXTRA_DIST = hand_display.c hand_display.h

all: hand_display.o

hand_display.o: hand_display.c hand_display.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean-local:
	rm -f *.o tags
