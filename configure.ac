dnl
dnl This is the configuration file for GNU oleo.
dnl
dnl $Id: configure.in,v 1.96 2011/07/05 06:08:43 delqn Exp $
dnl
dnl Process this file with autoconf to produce a configure script.
dnl
dnl Actually, it's configure.ac now. MAL 2024

dnl
dnl Need to be processed with Autoconf >= 2.13 ...sike! MAL 2024
dnl
dnlAC_PREREQ(2.13)

AC_INIT([oleo],[1.99.17])
AM_INIT_AUTOMAKE
AM_MAINTAINER_MODE([enable])
GNU_PACKAGE="GNU Oleo"
AC_DEFINE([GNU_PACKAGE], ["GNU Oleo"], [GNU Oleo])

dnl
dnl Use of this macro will keep unexpected things from happening if
dnl you're not familiar with automake.
dnl
dnl    if test -z "$enable_maintainer_mode"
dnl    then
dnl        enable_maintainer_mode=yes
dnl    fi
AM_MAINTAINER_MODE

dnl Define available languages
dnl This is supposedly obsolete.
dnl ALL_LINGUAS="en nl fr"

dnl AC_ARG_PROGRAM
AC_CONFIG_HEADERS([config.h])

dnl
dnl Find a C compiler, detect strange systems.
dnl The order is important here.
dnl
AC_PROG_CC
AC_USE_SYSTEM_EXTENSIONS
AC_SEARCH_LIBS([strerror],[cposix])

AC_CANONICAL_HOST
case $host_os in
  *cygwin* ) CYGWIN=yes;;
	 * ) CYGWIN=no;;
esac

AC_EXEEXT

dnl
dnl for intl/
dnl
AC_CHECK_SIZEOF(short, 2)
AC_CHECK_SIZEOF(int, 4)
AC_CHECK_SIZEOF(long, 4)

dnl
dnl Dmalloc is a library to debug memory allocation.
dnl Get it from www.dmalloc.com
dnl
AM_WITH_DMALLOC

dnl
dnl Now at last we can do checks for programs.
dnl
AC_PROG_YACC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_RANLIB

AC_CHECK_PROG(HAVE_MAKEINFO, makeinfo, yes)
AM_CONDITIONAL(HaveMakeinfo, test x$HAVE_MAKEINFO = xyes)

dnl
dnl Get perl's location
dnl
AC_PATH_PROG(HAVE_PERL, perl)
AC_SUBST(HAVE_PERL)
AM_CONDITIONAL(HavePerl, test x$HAVE_PERL != x)


dnl
dnl Dirty trick
dnl
if test -d /usr/local ; then
dnl	CFLAGS="-I/usr/local/include $CFLAGS "
	INCLUDES="-I/usr/local/include $INCLUDES "
	LIBS="-L/usr/local/lib $LIBS "
fi

dnl
dnl How good is the C compiler ?
dnl
dnl AM_C_PROTOTYPES MAL 2024

dnl
dnl Checks for libraries.
dnl
AC_CHECK_LIB(ncurses, waddch, LIBS="$LIBS -lncurses",
	AC_CHECK_LIB(curses, addch, ,
		AC_CHECK_LIB(termlib, tgetent, ,
			AC_CHECK_LIB(termcap, tgetent))))

AC_CHECK_HEADERS(ncurses.h, break, ac_cv_lib_ncurses_waddch="no")

dnl Check for sys_errlist[] and sys_nerr, check for declaration
AC_DEFINE([HAVE_SYS_ERRLIST], [], [Define if `sys_errlist' and `sys_nerr' are declared. Normally in <errno.h>.])

AC_MSG_CHECKING([for sys_errlist and sys_nerr])
AC_CACHE_VAL(oleo_cv_sys_errlist,
[AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <errno.h>
#include <stdio.h>]], [[  char *msg = sys_errlist[sys_nerr - 1];]])],[oleo_cv_sys_errlist=yes],[oleo_cv_sys_errlist=no])])dnl
      AC_MSG_RESULT($oleo_cv_sys_errlist)
      if test $oleo_cv_sys_errlist = yes; then
      AC_DEFINE(HAVE_SYS_ERRLIST)
      fi

dnl
dnl This library is required on FreeBSD.
dnl
AC_CHECK_LIB(mytinfo, tputs)

dnl
dnl Good i18n on FreeBSD requires the XPG4 library
dnl
AC_CHECK_LIB(xpg4, setlocale)

dnl
dnl Check existence of a working math library
dnl	The good old check with sqrtf fails on some systems.
dnl	Is there a function that works on all ?
dnl	Yes: "main" just detects presence of the library.
dnl
AC_CHECK_LIB(m, main)

dnl
dnl Checks for X11
dnl
AC_PATH_XTRA

dnl There are cases where motif may be installed on a system, but X may not
dnl be (Default on hppa1.1-hp-hpux10.20) therefor this this must all be wrapped
if test "$no_x" = ""; then
    dnl Check for Motif
    AC_ARG_WITH(motif,
        [  --without-motif              do not use Motif, even if detected],
        [   case "${withval}" in
            y | ye | yes ) usemotif=yes ;;
            n | no )       usemotif=no ;;
            * )            usemotif=yes ;;
            esac],
        [   with_motif=yes])

    if test "$with_motif" = "yes"; then
        AC_FIND_XBAE
        if test "$with_xbae" = "no" ; then
            AC_MSG_ERROR([Cannot build Motif interface without Xbae])
            with_motif=no
        fi

        ICE_FIND_XmHTML
        AC_FIND_XLT

        if test "x$XLT_LIBS" != "x"; then
            AC_DEFINE([HAVE_SciPlot_H], [], [SciPlot])
        else
            ICE_FIND_SciPlot
        fi

        AC_CHECK_LIB(Xpm, main)
    else
        AC_DEFINE([HAVE_MOTIF], [], [Define if you have Motif])
        AC_DEFINE([HAVE_XmHTML_H], [], [Define if you have XmHTML_H])
        AC_DEFINE([HAVE_XBAE], [], [Define if you have Xbae])
        AC_DEFINE([HAVE_XLT], [], [Define if you have Xlt])
        AC_DEFINE([HAVE_XLT_FONTCHOOSER], [], [Define if you have Xlt font chooser])
        AC_DEFINE([HAVE_XPM], [], [Define if you have Xpm])
        AC_DEFINE([HAVE_LIB_XP], [], [Define if you have libXp])
    fi

    if test "$with_Xbae" = "no" -a "$with_motif" = "yes" ; then
        AC_MSG_ERROR([Cannot build Motif interface without Xbae])
    fi
    if test "$with_Xbae" = "yes" -a "$with_motif" = "no" ; then
        AC_MSG_ERROR([Cannot use Xbae without Motif])
    fi
fi dnl "$no_x" = "yes"

dnl
dnl This is an automake variable which allows you to use conditionals
dnl in makefiles. We'll use it in the src/ directory to figure out
dnl which files to compile.
dnl
AM_CONDITIONAL(UseMotif, test x$with_motif = xyes)

dnl
dnl Check whether a recent version of GNU PlotUtils is around
dnl	This needs to be done after the X and Motif tests, because
dnl	the libplot from GNU plotutils sometimes requires linking the
dnl	X and Motif libraries
dnl
AC_CHECK_LIB([plot], [pl_openpl_r], [
    AC_DEFINE([HAVE_LIBPLOT], [1], [GNU libplot])
    LIBS="$LIBS -lplot"
])

AC_CHECK_LIB([sciplot], [sp_begin_plot], [
    AC_DEFINE([HAVE_LIBSCIPLOT], [1], [SciPlot])
])

dnl Checks for Guile
dnl AC_CHECK_LIB(guile, scm_boot_guile)

dnl Checks if -DDEBUG is requested
AC_ARG_WITH(debug,
[  --with-debug                 Send debugging info to stderr],
[              case "${withval}" in
                 y | ye | yes )         usedebug=yes ;;
                 n | no )               usedebug=no ;;
                 * )                    usedebug=yes ;;
               esac],
[              with_debug=no])

if test $with_debug = yes; then
  CFLAGS="$CFLAGS -DDEBUG"
  if test "x$GCC" = "xyes"; then
    if test -z "`echo "$CFLAGS" | grep "\-Wall" 2> /dev/null`"; then
      CFLAGS="$CFLAGS -Wall"
    fi
  fi
fi
           
dnl
dnl Checks for header files.
dnl
AC_HEADER_DIRENT
AC_CHECK_MEMBERS([struct dirent.d_ino], [D_INO_IN_DIRENT=1], [D_INO_IN_DIRENT=0], [#include <dirent.h>])
AC_DEFINE_UNQUOTED([D_INO_IN_DIRENT], [$D_INO_IN_DIRENT], [Define if there is a member named d_ino in the struct describing directory headers.])
# Autoupdate added the next two lines to ensure that your configure
# script's behavior did not change.  They are probably safe to remove.
AC_CHECK_INCLUDES_DEFAULT
AC_PROG_EGREP

AC_CHECK_HEADERS(fcntl.h limits.h malloc.h strings.h sys/time.h unistd.h)
AC_CHECK_HEADERS(time.h math.h libintl.h utime.h)
AC_CHECK_HEADERS(afs/afs.h)

AC_CHECK_HEADER([locale.h], [AC_CHECK_DECLS([LC_MESSAGES], [], [], [#include <locale.h>])])
if test "$ac_cv_have_decl_LC_MESSAGES" = "yes"; then
  HAVE_LC_MESSAGES=1
else
  HAVE_LC_MESSAGES=0
fi
AC_DEFINE_UNQUOTED([HAVE_LC_MESSAGES], [$HAVE_LC_MESSAGES], [Define if your locale.h file contains LC_MESSAGES.])

dnl
dnl
AC_CHECK_FUNC(XltCreateFontChooser, AC_DEFINE(HAVE_XLT_FONTCHOOSER))

AC_CHECK_DECLS([TIOCGWINSZ], [], [], [#include <sys/ioctl.h>])
if test "$ac_cv_have_decl_TIOCGWINSZ" = "yes"; then
  GWINSZ_IN_SYS_IOCTL=1
else
  GWINSZ_IN_SYS_IOCTL=0
fi
AC_DEFINE_UNQUOTED([GWINSZ_IN_SYS_IOCTL], [$GWINSZ_IN_SYS_IOCTL], [Define if your system defines TIOCGWINSZ in sys/ioctl.h.])

dnl
dnl GNU GSL (GNU Scientific Library)
dnl
AC_CHECK_LIB(gsl, gsl_error,
    [LIBS="$LIBS -lgsl -lgslcblas"
    AC_DEFINE([HAVE_GSL], [1], [GNU Scientific Library (GSL)])],
    [],
    [-lgslcblas])


dnl
dnl MySQL client library ?
dnl
AC_CHECK_LIB(mysqlclient, mysql_real_connect)
AM_CONDITIONAL(HaveMySQL, test x$ac_cv_lib_mysqlclient_mysql_real_connect = xyes)

dnl
dnl Checks for typedefs, structures, and compiler characteristics.
dnl
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T
AC_CHECK_HEADERS_ONCE([sys/time.h])
# Obsolete code to be removed.
if test $ac_cv_header_sys_time_h = yes; then
  AC_DEFINE([TIME_WITH_SYS_TIME],[1],[Define to 1 if you can safely include both <sys/time.h>
	     and <time.h>.  This macro is obsolete.])
fi
# End of obsolete code.

AC_STRUCT_TM
AC_STRUCT_TIMEZONE

AC_CHECK_MEMBERS([struct stat.st_dm_mode], [HAVE_ST_DM_MODE=1], [HAVE_ST_DM_MODE=0])
AC_DEFINE_UNQUOTED([HAVE_ST_DM_MODE], [$HAVE_ST_DM_MODE], [Define if your struct stat has st_dm_mode.])

dnl
dnl Checks for library functions.
dnl
AC_FUNC_ALLOCA
AC_CACHE_CHECK([return type of signal handlers],[ac_cv_type_signal],[AC_COMPILE_IFELSE(
[AC_LANG_PROGRAM([#include <sys/types.h>
#include <signal.h>
],
		 [return *(signal (0, 0)) (0) == 1;])],
		   [ac_cv_type_signal=int],
		   [ac_cv_type_signal=void])])
AC_DEFINE_UNQUOTED([RETSIGTYPE],[$ac_cv_type_signal],[Define as the return type of signal handlers
		    ('int' or 'void').])

AC_FUNC_STRFTIME
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(ftime gethostname gettimeofday mktime select strdup strstr)
AC_CHECK_FUNCS(putenv strcasecmp strerror getcwd rename regcomp)
AC_CHECK_FUNCS(stpcpy)
AC_CHECK_FUNCS(getgroups strftime fnmatch catgets gettext obstack)
AC_FUNC_MKTIME

AC_CHECK_TYPE(ino_t)

dnl
dnl Filesystem routines
dnl
AC_CHECK_FUNCS(fread getfsstat getmnt getmntent getmntent_r getmntinfo)
AC_CHECK_FUNCS(listmntent mntctl)
AC_CHECK_HEADERS(sys/fstyp.h sys/mntctl.h)
if test "$ac_cv_func_getfsstat" = "yes"; then
    MOUNTED_METHOD=MOUNTED_GETFSSTAT
elif test "$ac_cv_func_getmnt" = "yes"; then
    MOUNTED_METHOD=MOUNTED_GETMNT
elif test "$ac_cv_func_getmntent" = "yes"; then
    MOUNTED_METHOD=MOUNTED_GETMNTENT1
elif test "$ac_cv_func_getmntent_r" = "yes"; then
    MOUNTED_METHOD=MOUNTED_GETMNTENT2
elif test "$ac_cv_func_getmntinfo" = "yes"; then
    MOUNTED_METHOD=MOUNTED_GETMNTINFO
elif test "$ac_cv_func_listmntent" = "yes"; then
    MOUNTED_METHOD=MOUNTED_LISTMNTENT
elif test "$ac_cv_func_mntctl" = "yes" && test "$ac_cv_header_sys_mntctl_h" = "yes"; then
    MOUNTED_METHOD=MOUNTED_VMOUNT
elif test "$ac_cv_header_sys_fstyp_h" = "yes"; then
    MOUNTED_METHOD=MOUNTED_FREAD_FSTYP
else
    MOUNTED_METHOD=MOUNTED_FREAD
fi
AC_DEFINE_UNQUOTED([MOUNTED_METHOD], ["$MOUNTED_METHOD"], [Define one of the following to indicate how a program can obtain a list of mounted filesystems.])

AC_CHECK_FUNCS([statfs statvfs])
AC_CHECK_DECLS([struct statfs.f_flags])
AC_CHECK_DECLS([struct statfs.f_bsize])
AC_CHECK_DECLS([struct statfs.f_fsize])
AC_CHECK_DECLS([struct fs_data])
AC_CHECK_FUNCS([statfs], [], [], [#include <sys/param.h>])
AC_CHECK_FUNCS([statvfs], [], [], [#include <sys/statvfs.h>])
AC_CHECK_DEFINE([STATFS_TRUNCATES_BLOCK_COUNTS], [], [], [#include <sys/param.h>])


dnl
dnl CUPS is the Common UNIX Printing System
dnl	http://www.cups.org
dnl
AC_CHECK_LIB(cups, cupsGetPrinters)

dnl
dnl
dnl Xbase support (to read DBF files).
dnl	You'll need to uncomment the lines below for Xbase support,
dnl	as is indicated in the FAQ.
dnl
AC_PROG_CXX
dnl AC_PROG_CXX_NONFATAL
dnl
dnl End Xbase support lines
dnl
if test "$CXX" != "no"; then
  AC_LANG_SAVE
  AC_LANG([C++])
  AC_CHECK_LIB(xbase, main)
  if test x$ac_cv_lib_xbase_main = xno; then
    AC_CHECK_LIB(xdb, main)
    AM_CONDITIONAL(HaveXbase, test x$ac_cv_lib_xdb_main = xyes)
  else
    AM_CONDITIONAL(HaveXbase, test x$ac_cv_lib_xbase_main = xyes)
  fi
  AC_LANG_RESTORE
else
  AM_CONDITIONAL(HaveXbase, false)
fi

dnl See if we're GNU Gettext friendly
dnl AM_GNU_GETTEXT([external])

dnl
dnl This used to cause all kinds of problems in an earlier revision
dnl	of oleo. (Mostly because of using earlier auto* tools.)
dnl

dnl
dnl Pass information about where we install to the source, to be
dnl	able to read AFM files. Note this doesn't work if you
dnl	install via "make install DESTDIR=/xxx"
dnl
if test "$prefix" != "NONE"; then
	BUILD_PREFIX=$prefix
else
	BUILD_PREFIX=$ac_default_prefix
fi
AC_SUBST(BUILD_PREFIX)

dnl
dnl
dnl

dnl AC_SUBST(LIBOBJS)

AC_SUBST(include_motif)
AC_SUBST(link_motif)
AC_SUBST(include_xbae)
AC_SUBST(link_xbae)
AC_SUBST(include_xmhtml)
AC_SUBST(link_xmhtml)
AC_SUBST(include_sciplot)
AC_SUBST(link_sciplot)

dnl po/Makefile.in 
AC_CONFIG_FILES([Makefile oleobug doc/Makefile lib/Makefile src/Makefile m4/Makefile Xresources/Makefile examples/Makefile oleo.spec afm/Makefile ])
AC_OUTPUT 

