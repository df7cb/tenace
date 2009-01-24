CC = @CC@
CFLAGS += @PACKAGE_CFLAGS@
LDFLAGS += @PACKAGE_LIBS@

EXTRA_DIST = hand_display.c hand_display.h

all: hand_display.o

hand_display.o: hand_display.c hand_display.h int_int_marshal.h
	$(CC) $(CFLAGS) -I. -c -o $@ $<

int_int_marshal.h:
	echo VOID:INT,INT | glib-genmarshal --header --body --skip-source > $@

clean-local:
	rm -f *.o tags int_int_marshal.h
