dnl
dnl
dnl AC_LINK_X : find X11 (minimal file to OPTIONALLY link Oleo
dnl	with X11 libraries.
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
dnl The variable provided is :
dnl	link_x			(e.g. -L/usr/X11R6/lib -lX11 -lXt)
dnl
dnl The link_x variable should be fit to put on the application's
dnl link line in the Makefile.
dnl
dnl Oleo CVS $Id: x11.m4,v 1.1 1999/02/10 02:01:18 bennett Exp $
dnl
AC_DEFUN(AC_LINK_X,
[if test "$with_x" = "yes"; then
  X_LIBS="${X_LIBS} -lX11 -lXt"
fi])

