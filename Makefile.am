#
# This is the main Makefile for GNU Oleo.
#
# Process this file with automake to produce Makefile.in
#
# $Header: /home/matt/cvs/oleo/oleo/Makefile.am,v 1.18 1999/02/10 22:23:04 danny Exp $
#
AUTOMAKE_OPTIONS = gnu 1.4 readme-alpha
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = doc lib intl src m4 po Xresources

# Remove this file here (it is created via configure), not from within intl.
DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = oleobug.in

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`
