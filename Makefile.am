## Process this file with automake to produce Makefile.in

ACLOCAL_AMFLAGS = -I m4
SUBDIRS = contrib dds doc examples handdisplay lib src po

dist_pkgdata_DATA = tenace.desktop tenace.png tenace.ui tenace.glade magic.ui

EXTRA_DIST = \
	     README.md \
	     autogen.sh \
	     dds/patches/dds.cpp-autoconfig \
	     dds/patches/dds.cpp-fixes \
	     dds/patches/dds.cpp-openmp \
	     dds/patches/dll.h-extern_c \
	     dds/patches/dll.h-__int64 \
	     dds/patches/series \
	     dds/patches/skip-dll-build \
	     .travis.yml

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

