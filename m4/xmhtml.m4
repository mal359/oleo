dnl
dnl
dnl ICE_FIND_XmHTML
dnl
dnl Adapted from a macro by Andreas Zeller.
dnl
AC_DEFUN(ICE_FIND_XmHTML,
[
AC_REQUIRE([AC_PATH_XTRA])
xmhtml_includes=
xmhtml_libraries=
AC_ARG_WITH(XmHTML,
[  --without-XmHTML         do not use XmHTML widgets])
dnl Treat --without-XmHTML like
dnl --without-XmHTML-includes --without-XmHTML-libraries.
if test "$with_XmHTML" = "no"
then
xmhtml_includes=none
xmhtml_libraries=none
fi
AC_ARG_WITH(xmhtml-includes,
[  --with-xmhtml-includes=DIR    Motif include files are in DIR],
xmhtml_includes="$withval")
AC_ARG_WITH(xmhtml-libraries,
[  --with-xmhtml-libraries=DIR   Motif libraries are in DIR],
xmhtml_libraries="$withval")
AC_MSG_CHECKING(for XmHTML)
#
#
# Search the include files.
#
if test "$xmhtml_includes" = ""; then
AC_CACHE_VAL(ice_cv_xmhtml_includes,
[
ice_xmhtml_save_LIBS="$LIBS"
ice_xmhtml_save_CFLAGS="$CFLAGS"
ice_xmhtml_save_CPPFLAGS="$CPPFLAGS"
ice_xmhtml_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_COMPILE([#include <XmHTML/XmHTML.h>],[int a;],
[
# XmHTML/XmHTML.h is in the standard search path.
ice_cv_xmhtml_includes=
],
[
# XmHTML/XmHTML.h is not in the standard search path.
# Locate it and put its directory in `xmhtml_includes'
#
# /usr/include/Motif* are used on HP-UX (Motif).
# /usr/include/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_xmhtml_includes="none"
for dir in "$x_includes" "${prefix}/include" /usr/include /usr/local/include \
           /usr/include/Motif2.0 /usr/include/Motif1.2 /usr/include/Motif1.1 \
           /usr/include/X11R6 /usr/include/X11R5 /usr/include/X11R4 \
           /usr/dt/include /usr/openwin/include \
           /usr/dt/*/include /opt/*/include /usr/include/Motif* \
	   /home/XmHTML/include /usr/XmHTML/include /opt/XmHTML/include \
	   /home/XmHTML*/include /usr/XmHTML*/include /opt/XmHTML*/include \
           "${prefix}"/*/include /usr/*/include /usr/local/*/include \
           "${prefix}"/include/* /usr/include/* /usr/local/include/*; do
if test -f "$dir/XmHTML/XmHTML.h"; then
	ice_cv_xmhtml_includes="$dir"
	break
fi
done
])
#
LIBS="$ice_xmhtml_save_LIBS"
CFLAGS="$ice_xmhtml_save_CFLAGS"
CPPFLAGS="$ice_xmhtml_save_CPPFLAGS"
LDFLAGS="$ice_xmhtml_save_LDFLAGS"
])
xmhtml_includes="$ice_cv_xmhtml_includes"
fi
#
#
# Now for the libraries.
#
if test "$xmhtml_libraries" = ""; then
AC_CACHE_VAL(ice_cv_xmhtml_libraries,
[
ice_xmhtml_save_LIBS="$LIBS"
ice_xmhtml_save_CFLAGS="$CFLAGS"
ice_xmhtml_save_CPPFLAGS="$CPPFLAGS"
ice_xmhtml_save_LDFLAGS="$LDFLAGS"
#
LIBS="$X_PRE_LIBS -lXmHTML -lXm -lXt -lX11 $X_EXTRA_LIBS $LIBS"
CFLAGS="$X_CFLAGS $CFLAGS"
CPPFLAGS="$X_CFLAGS $CPPFLAGS"
LDFLAGS="$X_LIBS $LDFLAGS"
#
AC_TRY_LINK([#include <XmHTML/XmHTML.h>],[XmCreateHTML();],
[
# libXm.a is in the standard search path.
ice_cv_xmhtml_libraries=
],
[
# libXm.a is not in the standard search path.
# Locate it and put its directory in `xmhtml_libraries'
#
# /usr/lib/Motif* are used on HP-UX (Motif).
# /usr/lib/X11* are used on HP-UX (X and Athena).
# /usr/dt is used on Solaris (Motif).
# /usr/lesstif is used on Linux (Lesstif).
# /usr/openwin is used on Solaris (X and Athena).
# Other directories are just guesses.
ice_cv_xmhtml_libraries="none"
for dir in "$x_libraries" "${prefix}/lib" /usr/lib /usr/local/lib \
           /usr/lib/Motif2.0 /usr/lib/Motif1.2 /usr/lib/Motif1.1 \
           /usr/lib/X11R6 /usr/lib/X11R5 /usr/lib/X11R4 /usr/lib/X11 \
           /usr/dt/lib /usr/openwin/lib \
           /usr/dt/*/lib /opt/*/lib /usr/lib/Motif* \
           /usr/lesstif*/lib /usr/lib/Lesstif* \
	   /home/XmHTML/lib /usr/XmHTML/lib /opt/XmHTML/lib \
	   /home/XmHTML*/lib /usr/XmHTML*/lib /opt/XmHTML*/lib \
           "${prefix}"/*/lib /usr/*/lib /usr/local/*/lib \
           "${prefix}"/lib/* /usr/lib/* /usr/local/lib/*; do
if test -d "$dir" && test "`ls $dir/libXmHTML.* 2> /dev/null`" != ""; then
	ice_cv_xmhtml_libraries="$dir"
	break
fi
done
])
#
LIBS="$ice_xmhtml_save_LIBS"
CFLAGS="$ice_xmhtml_save_CFLAGS"
CPPFLAGS="$ice_xmhtml_save_CPPFLAGS"
LDFLAGS="$ice_xmhtml_save_LDFLAGS"
])
#
xmhtml_libraries="$ice_cv_xmhtml_libraries"
fi
#
# Provide an easier way to link
#
if test "$xmhtml_includes" != "" && test "$xmhtml_includes" != "$x_includes" && test "$xmhtml_includes" != "none"; then
	link_xmhtml="-L$xmhtml_libraries -lXmHTML -ljpeg"
	include_xmhtml="-I$xmhtml_includes"
	AC_DEFINE(HAVE_XmHTML_H)
fi
#
AC_SUBST(include_xmhtml)
AC_SUBST(link_xmhtml)
#
#
#
xmhtml_libraries_result="$xmhtml_libraries"
xmhtml_includes_result="$xmhtml_includes"
test "$xmhtml_libraries_result" = "" && xmhtml_libraries_result="in default path"
test "$xmhtml_includes_result" = "" && xmhtml_includes_result="in default path"
test "$xmhtml_libraries_result" = "none" && xmhtml_libraries_result="(none)"
test "$xmhtml_includes_result" = "none" && xmhtml_includes_result="(none)"
AC_MSG_RESULT(
  [libraries $xmhtml_libraries_result, headers $xmhtml_includes_result])
])dnl
