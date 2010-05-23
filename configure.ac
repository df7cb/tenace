dnl Process this file with autoconf to produce a configure script.

AC_INIT(tenace, 0.11, [cb@df7cb.de])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

AC_ISC_POSIX
AC_PROG_CC
gl_EARLY
AC_PROG_INTLTOOL
AM_PROG_LIBTOOL
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_CHECK_LIB([dds], [SolveBoard], [havelibdds=yes],
	     [echo Using internal dds library
	      CFLAGS="$CFLAGS -I../dds"
	      LDFLAGS="$LDFLAGS -L../dds"], [])
AM_CONDITIONAL(HAVE_LIBDDS, [test x$havelibdds = xyes])

pkg_modules="gtk+-2.0 >= 2.0.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

GETTEXT_PACKAGE=tenace
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE],["$GETTEXT_PACKAGE"], [tenace])

dnl Add the languages which your application supports here.
ALL_LINGUAS="de en"
AM_GLIB_GNU_GETTEXT
dnl Source is in utf-8
XGETTEXT="xgettext --from-code=utf-8"

dnl IT_PROG_INTLTOOL
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
