## Process this file with automake to produce Makefile.in
noinst_LIBRARIES = liboleo.a

liboleo_a_SOURCES = getdate.y getopt.c getopt1.c xmalloc.c error.c \
	random.c

noinst_HEADERS = getopt.h error.h getdate.h obstack.h mktime.c obstack.c 

INCLUDES = -I.. -I$(srcdir) -I../intl

liboleo_a_LIBADD = @LIBOBJS@

