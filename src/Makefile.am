AUTOMAKE_OPTIONS = ../lib/ansi2knr dejagnu

bin_PROGRAMS = oleo

if UseMotif
GUI = io-motif.c appres.c
else
GUI = io-gtk.c
endif

oleo_SOURCES = parse.y byte-compile.c eval.c ref.c \
 decompile.c sort.c regions.c utils.c lists.c\
 io-term.c io-utils.c io-x11.c window.c io-edit.c\
 panic.c line.c key.c format.c funcs.c \
 hash.c ir.c io-curses.c font.c display.c print.c init.c \
 sylk.c oleofile.c sc.c list.c busi.c string.c cells.c \
 cmd.c basic.c args.c \
 stub.c input.c info.c help.c graph.c forminfo.c \
 posixtm.y date.c xrdb.c $(GUI)

if UseMotif
# oleo_SOURCES += io-motif.c appres.c
oleo_LDADD = @INTLLIBS@ @link_motif@ @link_xmhtml@ @link_xbae@ \
               @link_sciplot@ ../lib/liboleo.a

INCLUDES = -I.. -I$(srcdir) -I$(top_srcdir)/lib -I../intl \
	   ${X_CFLAGS} \
           @include_motif@ @include_xbae@ @include_xmhtml@ @include_sciplot@

gpl.h:	$(top_srcdir)/COPYING gpl.sed
	sed -f gpl.sed <$(top_srcdir)/COPYING >gpl.h

io-motif.o:	gpl.h fallback.h
else
# oleo_SOURCES += io-gtk.c
oleo_LDADD = @INTLLIBS@ @GTK_LIBS@ ../lib/liboleo.a

INCLUDES = -I.. -I$(srcdir) -I$(top_srcdir)/lib -I../intl
CFLAGS = @CFLAGS@ @GTK_CFLAGS@
endif

oleo_LDADD += ${X_LIBS} -lXt -lX11

noinst_HEADERS = args.h basic.h byte-compile.h cell.h cmd.h decompile.h \
 defun.h defuns.h display.h errors.h eval.h font.h format.h forminfo.h \
 funcdef.h funcs.h global.h graph.h hash.h help.h info.h init.h \
 input.h io-abstract.h io-curses.h io-edit.h io-generic.h io-term.h \
 io-utils.h io-x11.h ir.h key.h line.h lists.h node.h oleofile.h \
 panic.h parse.h print.h proto.h ref.h regions.h sc.h stub.h stubs.h sylk.h \
 sysdef.h utils.h window.h list.h ir.h io-gtk.h sort.h \
 io-motif.h appres.h fallback.h

EXTRA_DIST =	gpl.sed
