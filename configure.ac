dnl Process this file with autoconf to produce a configure script.

AC_INIT(tenace, 0.15, [cb@df7cb.de])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

AC_ISC_POSIX
AC_PROG_CC
gl_EARLY
AM_PROG_LIBTOOL
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_CHECK_LIB([dds], [SolveBoard], [havelibdds=yes],
	     [echo Using internal dds library
	      CFLAGS="$CFLAGS -I../dds"
	      LDFLAGS="$LDFLAGS -L../dds"], [-lgomp])
AM_CONDITIONAL(HAVE_LIBDDS, [test x$havelibdds = xyes])

pkg_modules="gtk+-2.0 >= 2.0.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

IT_PROG_INTLTOOL([0.35.0])

GETTEXT_PACKAGE=tenace
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],
		                      [The domain to use with gettext])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.17])

PROGRAMNAME_LOCALEDIR=[${datadir}/locale]
AC_SUBST(PROGRAMNAME_LOCALEDIR)

gl_INIT

AC_CONFIG_FILES([
Makefile
contrib/Makefile
dds/Makefile
doc/Makefile
examples/Makefile
handdisplay/Makefile
lib/Makefile
src/Makefile
po/Makefile.in
])
AC_OUTPUT
