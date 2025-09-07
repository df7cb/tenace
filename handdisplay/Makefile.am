CC = @CC@
CFLAGS += @PACKAGE_CFLAGS@
LDFLAGS += @PACKAGE_LIBS@

EXTRA_DIST = hand_display.c hand_display.h int_int_marshal.def

all: hand_display.o

hand_display.o: hand_display.c hand_display.h int_int_marshal.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -I. -c -o $@ $<

int_int_marshal.h: int_int_marshal.def
	glib-genmarshal --header $< > $@
	glib-genmarshal --body $< >> $@

clean-local:
	rm -f *.o tags int_int_marshal.h
