## Process this file with automake to produce Makefile.in
info_TEXINFOS = oleo.texi

TEXIS=		basics.texi bugs.texi cell-refs.texi entering-data.texi \
		expressions.texi functions.texi interface.texi keymaps.texi \
		macros.texi making-changes.texi options.texi print-plot.texi \
		programs.texi ranlib.texi style.texi texinfo.tex windows.texi

#		motif.texi

EXTRA_DIST=	$(TEXIS)
noinst_DATA =	oleo.html $(TEXIS)

oleo.info:	oleo.texi $(TEXIS)

htmldir=	$(prefix)/Oleo/html
html_DATA=	oleo.html

oleo.html:	oleo.texi ${TEXIS}
		texi2html oleo.texi
