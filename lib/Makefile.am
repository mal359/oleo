#
# $Header: /home/matt/cvs/oleo/oleo/lib/Makefile.am,v 1.8 1999/01/27 20:32:52 danny Exp $
#
# Process this file with automake to produce Makefile.in
#
noinst_LIBRARIES = liboleo.a

#liboleo_a_SOURCES = getdate.y getopt.c getopt1.c xmalloc.c error.c \
#	random.c
#noinst_HEADERS = getopt.h error.h get_date.h obstack.h mktime.c obstack.c 

liboleo_a_SOURCES = getdate.y getopt.c getopt1.c xmalloc.c random.c

noinst_HEADERS = getopt.h get_date.h obstack.h mktime.c obstack.c 

INCLUDES = -I.. -I$(srcdir) -I../intl

liboleo_a_LIBADD = @LIBOBJS@

