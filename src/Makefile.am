AUTOMAKE_OPTIONS = ../lib/ansi2knr

bin_PROGRAMS = oleo

oleo_SOURCES = parse.y byte-compile.c eval.c ref.c \
 decompile.c sort.c regions.c utils.c lists.c\
 io-term.c io-utils.c io-x11.c window.c io-edit.c\
 hash.c panic.c line.c key.c format.c funcs.c \
 ir.c io-curses.c font.c display.c print.c init.c \
 sylk.c oleofile.c sc.c list.c busi.c string.c cells.c \
 random.c cmd.c basic.c args.c \
 stub.c input.c info.c help.c graph.c forminfo.c \
 posixtm.y date.c xmalloc.c xrdb.c

noinst_HEADERS = args.h basic.h byte-compile.h cell.h cmd.h decompile.h \
 defun.h defuns.h display.h errors.h eval.h font.h format.h forminfo.h \
 funcdef.h funcs.h global.h graph.h hash.h help.h info.h init.h \
 input.h io-abstract.h io-curses.h io-edit.h io-generic.h io-term.h \
 io-utils.h io-x11.h ir.h key.h line.h lists.h node.h oleofile.h \
 panic.h parse.h print.h proto.h ref.h regions.h sc.h stub.h stubs.h sylk.h \
 sysdef.h utils.h window.h list.h ir.h 

# oleo_LDADD = $(X_PRE_LIBS) $(X_LIBS) -lX11
#oleo_LDADD = $(X_PRE_LIBS) $(X_LIBS)
# oleo_LDADD = @LIBOBJS@

oleo_LDADD = ../lib/liboleo.a

INCLUDES = $(X_CFLAGS)

