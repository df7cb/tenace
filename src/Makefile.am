## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = bridgehand

bridgehand_SOURCES = \
	main.c main.h \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	bridge.c bridge.h \
	board.c board.h \
	file.c file.h \
	functions.c functions.h \
	solve.c solve.h \
	window_card.c window_card.h

bridgehand_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -ldds

