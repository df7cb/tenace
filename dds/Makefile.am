if HAVE_LIBDDS
all: noop
else
all: libdds.a
endif

dds.c:
	ln -s dds.cpp $@

dds.h:
	ln -s dll.h dds.h

dds.o: dds.c dds.h
	$(CC) $(CFLAGS) -c -o $@ $<

libdds.a: dds.o
	ar rc $@ $^
	ranlib $@

clean-local:
	rm -f dds.o libdds.a dds.c dds.h

noop:
	@echo "Nothing to do here, using the system's libdds"
