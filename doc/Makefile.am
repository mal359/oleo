#
# $Header: /home/matt/cvs/oleo/oleo/doc/Makefile.am,v 1.14 2001/02/08 00:21:23 pw Exp $
#
# Process this file with automake to produce Makefile.in
#
info_TEXINFOS = oleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		expressions.texi functions.texi interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi \
		motif.texi dbms.texi

EXTRA_DIST=	$(TEXIS) oleo_web.html texi2html oleo1.png
noinst_DATA =	oleo_web.html $(TEXIS)

oleo.info:	oleo.texi $(TEXIS)

if HavePerl

htmldir=	$(prefix)/share/oleo
html_DATA=	oleo.html

oleo.html:	$(srcdir)/oleo.texi ${TEXIS} $(srcdir)/texi2html
		@HAVE_PERL@ $(srcdir)/texi2html $(srcdir)/oleo.texi
endif

DISTCLEANFILES = $(html_DATA)
MAINTAINERCLEANFILES = Makefile.in

