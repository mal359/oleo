#
# This is the main Makefile for GNU Oleo.
#
# Process this file with automake to produce Makefile.in
#
# $Header: /home/matt/cvs/oleo/oleo/Makefile.am,v 1.24 1999/03/16 22:57:18 danny Exp $
#
AUTOMAKE_OPTIONS = gnu 1.4 readme-alpha
ACLOCAL_AMFLAGS = -I m4

D=	`date +%G%m%d.%H%M%S`

SUBDIRS = doc lib intl src m4 po Xresources examples

# Remove this file here (it is created via configure), not from within intl.
DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = oleobug.in FAQ oleo.spec.in oleo.spec \
		ftp.gnudist ftp.alpha ftp.dist

release:	oleo-$(VERSION).tar.gz
	sed -e s/Old/$D/ <ftp.gnudist | ftp gnudist.gnu.org 
	sed -e s/version/$(VERSION)/ <ftp.alpha | ftp alpha.gnu.org 

release.alpha:	oleo-$(VERSION).tar.gz
	sed -e s/Old/$D/ <ftp.dist | ftp gnudist.gnu.org 

docdir=        $(prefix)/Oleo
doc_DATA=	AUTHORS FAQ

