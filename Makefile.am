## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = gnu 1.3

SUBDIRS = doc lib po intl src

EXTRA_DIST = oleobug.in

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`

