#
# This is the main Makefile for GNU Oleo.
#
# Process this file with automake to produce Makefile.in
#
# $Header: /home/matt/cvs/oleo/oleo/Makefile.am,v 1.20 1999/02/23 21:24:34 danny Exp $
#
AUTOMAKE_OPTIONS = gnu 1.4 readme-alpha
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = doc lib intl src m4 po Xresources

# Remove this file here (it is created via configure), not from within intl.
DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = oleobug.in FAQ

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`

docdir=        $(exec_prefix)/Oleo
doc_DATA=	AUTHORS FAQ
