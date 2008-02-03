dnl Process this file with autoconf to produce a configure script.

AC_INIT(tenace, 0.4, [cb@df7cb.de])
AC_CONFIG_AUX_DIR([config])
AM_INIT_AUTOMAKE([-Wall foreign])
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC

pkg_modules="gtk+-2.0 >= 2.12.0"
PKG_CHECK_MODULES(PACKAGE, [$pkg_modules])
AC_SUBST(PACKAGE_CFLAGS)
AC_SUBST(PACKAGE_LIBS)

GETTEXT_PACKAGE=tenace
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE,"$GETTEXT_PACKAGE", [Gettext package.])

dnl Add the languages which your application supports here.
ALL_LINGUAS="de en"
AM_GLIB_GNU_GETTEXT
dnl Source is in utf-8
XGETTEXT="xgettext --from-code=utf-8"

AC_OUTPUT([
Makefile
cards/Makefile
contrib/Makefile
doc/Makefile
examples/Makefile
handdisplay/Makefile
src/Makefile
po/Makefile.in
])
