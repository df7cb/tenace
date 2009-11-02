## Process this file with automake to produce Makefile.in

SUBDIRS = contrib dds doc examples handdisplay src po

dist_pkgdata_DATA = tenace.ui magic.ui

INTLTOOL_FILES = \
	intltool-extract \
	intltool-merge \
	intltool-update

EXTRA_DIST = autogen.sh $(INTLTOOL_FILES)

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(DESTDIR)$(pkgdatadir)/pixmaps; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(DESTDIR)$(pkgdatadir)/pixmaps; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

