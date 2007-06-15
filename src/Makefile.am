## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = tenace

tenace_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	bridge.c bridge.h \
	file.c file.h \
	functions.c functions.h \
	solve.c solve.h \
	window_bids.c window_bids.h \
	window_board.c window_board.h \
	window_card.c window_card.h \
	window_line_entry.c window_line_entry.h \
	window_play.c window_play.h

tenace_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -ldds

