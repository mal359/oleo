## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = gnu no-dependencies 1.2

SUBDIRS = src doc 

release:
	$(MAKE) dist distdir=$(PACKAGE)`date +"%y%m%d"`

