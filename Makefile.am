## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = gnu 1.2f

SUBDIRS = doc lib po intl src

EXTRA_DIST = oleobug.in

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`

