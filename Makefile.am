#
# This is the main Makefile for GNU Oleo.
#
# Process this file with automake to produce Makefile.in
#
# $Header: /home/matt/cvs/oleo/oleo/Makefile.am,v 1.27 1999/07/23 16:23:41 danny Exp $
#
AUTOMAKE_OPTIONS = gnu 1.4 readme-alpha
ACLOCAL_AMFLAGS = -I m4

D=	`date +%G%m%d.%H%M%S`

SUBDIRS = doc lib intl src m4 po Xresources examples

# Remove this file here (it is created via configure), not from within intl.
DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = oleobug.in FAQ oleo.spec.in oleo.spec \
		ftp.gnudist ftp.alpha ftp.dist

#
# Use this only for real end-user-ready releases of Oleo
#
release:	oleo-$(VERSION).tar.gz
	rcp oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	rcp oleo-$(VERSION).tar.gz gnudist.gnu.org:/home/ftp/gnu/oleo/oleo-$(VERSION).tar.gz
	rcp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog

#
# This can be used to put the files in place for alpha releases.
#
release.alpha:	oleo-$(VERSION).tar.gz
	rcp oleo-$(VERSION).tar.gz alpha.gnu.org:/fs/share/ftp/gnu/oleo/oleo-$(VERSION).tar.gz
	rcp oleo_web.html gnudist.gnu.org:/home/www/html/software/oleo/oleo.html
	rcp ChangeLog gnudist.gnu.org:/home/www/html/software/oleo/ChangeLog

docdir=        $(prefix)/Oleo
doc_DATA=	AUTHORS FAQ

