#
# $Header: /home/matt/cvs/oleo/oleo/src/Makefile.am,v 1.34 1999/06/04 08:01:57 danny Exp $
#
localedir = $(prefix)/share/locale

AUTOMAKE_OPTIONS = ../lib/ansi2knr dejagnu

EXTRA_DIST=	xbase.cpp oleo_xb.h

bin_PROGRAMS = oleo

if UseMotif
GUI = io-motif.c appres.c fallback.c oleo_icon.xpm
else
GUI =
endif

#
# Xbase support
#	Thanks to comments from Tom Tromey this works now.
#	Need to specify the link command depending on whether we have
#	a C++ compiler or not.
#
if HaveXbase
XBSRC=		xbase.cpp oleo_xb.h
CXXFLAGS=	$(CFLAGS)
oleo_LINK=	$(CXXLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
else
XBSRC=
oleo_LINK=	$(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@
endif

oleo_SOURCES = parse.y byte-compile.c eval.c ref.c \
 decompile.c sort.c regions.c utils.c lists.c\
 io-term.c io-utils.c io-x11.c window.c io-edit.c\
 line.c key.c format.c funcs.c \
 hash.c ir.c io-curses.c font.c display.c init.c \
 sylk.c oleofile.c sc.c list.c busi.c string.c cells.c \
 cmd.c basic.c args.c \
 stub.c input.c info.c help.c \
 graph.c plot.c \
 forminfo.c posixtm.y date.c xrdb.c $(GUI) $(XBSRC) \
 print.c postscript.c pcl.c epson.c prtext.c

if UseMotif
MYLD = @link_motif@ @link_xmhtml@ @link_xbae@ @link_sciplot@
MYINC = @include_motif@ @include_xbae@ @include_xmhtml@ @include_sciplot@
MYCFL = ${X_CFLAGS}

gpl.h:	$(top_srcdir)/COPYING $(srcdir)/gpl.sed
	sed -f $(srcdir)/gpl.sed <$(top_srcdir)/COPYING >gpl.h

io-motif.o:	gpl.h $(top_srcdir)/configure $(srcdir)/oleo_icon.xpm ../config.h

else
# MYLD = @GTK_LIBS@
# MYINC =
# MYCFL = @GTK_CFLAGS@
endif

oleo_LDADD = ${X_PRE_LIBS} ${MYLD} @INTLLIBS@ ../lib/liboleo.a ${X_LIBS}
INCLUDES = -I.. -I$(srcdir) -I$(top_srcdir)/lib -I../intl ${MYINC}
CFLAGS = @CFLAGS@ -DLOCALEDIR=\"$(localedir)\" ${MYCFL}

noinst_HEADERS = args.h basic.h byte-compile.h cell.h cmd.h decompile.h \
 defun.h defuns.h display.h errors.h eval.h font.h format.h forminfo.h \
 funcdef.h funcs.h global.h hash.h help.h info.h init.h \
 input.h \
 io-abstract.h io-curses.h io-edit.h io-generic.h io-term.h \
 io-utils.h io-x11.h \
 ir.h key.h line.h lists.h node.h oleofile.h \
 parse.h \
 graph.h oleo_plot.h \
 proto.h ref.h regions.h \
 sc.h stub.h stubs.h sylk.h \
 sysdef.h utils.h window.h list.h ir.h sort.h \
 io-motif.h appres.h \
 print.h pcl.h postscript.h epson.h prtext.h

EXTRA_DIST =	gpl.sed

io-term.o:	$(top_srcdir)/configure
utils.o:	$(top_srcdir)/configure
io-utils.o:	../config.h
plot.o:		../config.h
io-term.o:	../config.h
cmd.o:		../config.h
io-x11.o:	../config.h
lists.o:	../config.h
