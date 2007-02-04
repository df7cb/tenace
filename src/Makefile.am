## Process this file with automake to produce Makefile.in

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	@PACKAGE_CFLAGS@

bin_PROGRAMS = bridgehand

bridgehand_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h \
	bridge.c bridge.h \
	board.c board.h \
	solve.c solve.h

bridgehand_LDADD = @PACKAGE_LIBS@ $(INTLLIBS) -ldds

