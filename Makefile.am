## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = gnu 1.3

SUBDIRS = doc lib intl src m4 # po

# Remove this file here (it is created via configure), not from within intl.
DISTCLEANFILES = intl/libintl.h

EXTRA_DIST = oleobug.in

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`

