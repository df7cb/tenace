EXTRA_DIST = \
	DDS_alg_descr-revE3.rtf \
	GPL.txt \
	dds.cpp \
	dll.h \
	mode2.txt \
	readme.txt \
	release_notes.txt

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
	$(AR) rc $@ $^
	$(RANLIB) $@

clean-local:
	rm -f dds.o libdds.a dds.c dds.h

noop:
	@echo "Nothing to do here, using the system's libdds"
