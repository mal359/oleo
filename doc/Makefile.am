#
# $Header: /home/matt/cvs/oleo/oleo/doc/Makefile.am,v 1.7 1998/10/07 20:14:26 danny Exp $
#
# Process this file with automake to produce Makefile.in
#
info_TEXINFOS = oleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		expressions.texi functions.texi interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi \
		motif.texi

EXTRA_DIST=	$(TEXIS) oleo_web.html
noinst_DATA =	oleo_web.html $(TEXIS)

oleo.info:	oleo.texi $(TEXIS)

if HaveTexi2Html

htmldir=	$(prefix)/share/oleo
html_DATA=	oleo.html

oleo.html:	$(srcdir)/oleo.texi ${TEXIS}
		texi2html $(srcdir)/oleo.texi

endif
