/*
 *  $Id: io-motif.c,v 1.26 1999/03/02 20:04:03 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright (C) 1998 by the Free Software Foundation, Inc.
 *  Written by Danny Backx <danny@gnu.org>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

static char rcsid[] = "$Id: io-motif.c,v 1.26 1999/03/02 20:04:03 danny Exp $";

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/param.h>
#include <stdarg.h>

#ifdef	HAVE_LOCALE_H
#include <locale.h>
#endif
#include <libintl.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/XmAll.h>
#include <Xbae/Matrix.h>
#include <Xbae/Caption.h>

#if	HAVE_XLT
#include <Xlt/SciPlot.h>
#define	HAVE_SciPlot_H
#else
#if	HAVE_SciPlot_H
#include <SciPlot/SciPlot.h>
#endif
#endif

#if	HAVE_XBASE_H
#include "oleo_xb.h"
#endif

#include "global.h"
#include "utils.h"
#include "io-generic.h"
#include "io-edit.h"
#include "io-utils.h"
#include "io-term.h"
#include "cmd.h"
#include "window.h"
#include "line.h"
#include "font.h"
#include "lists.h"
#include "ir.h"
#include "display.h"
#include "io-abstract.h"
#include "regions.h"
#include "input.h"
#include "info.h"
#include "eval.h"
#include "basic.h"
#include "graph.h"
#include "print.h"

#include "io-motif.h"		/* To get warnings when inconsistent */

#if	HAVE_XmHTML_H
#include <XmHTML/XmHTML.h>
#endif

XtAppContext app;
Widget	toplevel, splash, SplashShell, plot;
Widget	mw, mat = NULL, mb, filemenu, editmenu, stylemenu,
	optionsmenu, helpmenu, graphmenu;
Widget	msgtext = NULL, statustf = NULL, formulatf = NULL;
Widget	fsd = NULL;
Widget	hd, html = NULL, gs = NULL;
Widget	FormatD = NULL;
Widget	PrintDialog = NULL;
Widget	DefaultFileDialog, DefaultFileShell = NULL;

static Widget	w;

/* Fallback resources, application resources */
extern char *fallback[];
#include "appres.h"

GnuSheetAppres	AppRes;

/* This is ancient history */
int	cell_font_point_size = 12;
static int block_on_getch = 1;

static char	input_buf[1024];
static int	input_buf_allocated = 1024;

static int	chars_buffered = 0;

/* Forward declarations */
void CancelTemplate(Widget w, XtPointer client, XtPointer call);
static void FixA0();

/*
 * This is used in two places.
 */
#define	CopyRightString							\
	_(								\
	"\nGNU Oleo is free software, you are welcome to\n"		\
	"distribute copies of it. See the file COPYING for the\n"	\
	"conditions.\n\n"						\
	"GNU Oleo comes with ABSOLUTELY NO WARRANTY."			\
	)

/*
 * Conditionally (only when the user wants it) print debugging info
 */
void Debug(char *src, char *fmt, ...)
{
	static char	*srclist = NULL;
	va_list	ap;
	int		printit = 0;

	if (! srclist)
		srclist = getenv("DEBUGSOURCES");

	if (srclist && strstr(src, srclist))
		printit = 1;

	if (printit) {
		va_start(ap, fmt);
		vfprintf(stderr, fmt, ap);
		va_end(ap);
	}
}

void MessageAppend(Boolean beep, char *fmt, ...)
{
	XmTextPosition	pos;
	int		i;
	va_list		ap;
	static char	s[256];

	if (msgtext == NULL || fmt == NULL)
		return;

	va_start(ap, fmt);
	vsprintf(s, fmt, ap);
	va_end(ap);

	if ((i = strlen(s)) == 0)
		return;

	pos = XmTextGetLastPosition(msgtext);
	XmTextInsert(msgtext, pos, s);

	if (s[i-1] != '\n') {
		pos = XmTextGetLastPosition(msgtext);
		XmTextInsert(msgtext, pos, "\n");
	}

	XmTextShowPosition(msgtext, XmTextGetLastPosition(msgtext));

	if (beep)
		XBell(XtDisplay(msgtext), 30);
}

/*
 * This just prints a message saying this functionality hasn't been
 * implemented yet.
 */
void none(Widget w, XtPointer client, XtPointer call)
{
	XmString	xms;
	char		*s = NULL;

	XtVaGetValues(w, XmNlabelString, &xms, NULL);
	if (XmStringGetLtoR(xms, XmFONTLIST_DEFAULT_TAG, &s)) {
		MessageAppend(False, "%s: not implemented yet\n", s);
		XtFree(s);
	}
	XmStringFree(xms);
}

void PrintDebug(Widget w, XtPointer client, XtPointer call)
{
	int	i, j;

	for (i=1; i<6; i++) {
		fprintf(stderr, "[%d]\t", i);
		for (j=1; j<6; j++) {
			fprintf(stderr, "%s\t", cell_value_string(i, j, True));
		}
		fprintf(stderr, "\n");
	}
}

/****************************************************************
 *								*
 *		Range Selection Support				*
 *								*
 ****************************************************************/

Widget		ActiveRangeSelectionWidget = NULL;
struct rng	selection_range;

void FocusCB(Widget w, XtPointer client, XtPointer call)
{
	ActiveRangeSelectionWidget = w;
}

void LoseFocusCB(Widget w, XtPointer client, XtPointer call)
{
#if 0
	if (ActiveRangeSelectionWidget == w)
		ActiveRangeSelectionWidget = NULL;
#endif
}

void SelectCellCB(Widget w, XtPointer client, XtPointer call)
{
	char	*s;
	XbaeMatrixSelectCellCallbackStruct *cbp =
		(XbaeMatrixSelectCellCallbackStruct *)call;

#if 0
	fprintf(stderr, "SelectCell(%s, %d %d)\n",
		cbp->params[0], cbp->row, cbp->column);
#endif

	if (cbp->num_params >= 1 && strcmp(cbp->params[0], "start") == 0) {
		selection_range.lr = cbp->row + 1;
		selection_range.lc = cbp->column + 1;
	} else if (cbp->num_params >= 1 && strcmp(cbp->params[0], "end") == 0) {
		selection_range.hr = cbp->row + 1;
		selection_range.hc = cbp->column + 1;

		if (ActiveRangeSelectionWidget &&
				XmIsTextField(ActiveRangeSelectionWidget)) {
			s = range_name(&selection_range);
			XmTextFieldSetString(ActiveRangeSelectionWidget, s);
		}
	}
}

void RegisterRangeSelector(Widget w)
{
	XtAddCallback(w, XmNfocusCallback, FocusCB, NULL);
	XtAddCallback(w, XmNlosingFocusCallback, LoseFocusCB, NULL);
}

/****************************************************************
 *								*
 *		Generic Helper Functions			*
 *								*
 ****************************************************************/
void
PopDownHelpCB(Widget widget, XtPointer client, XtPointer call)
{
	XtPopdown((Widget)client);
}

void MotifUpdateDisplay()
{
        XbaeMatrixRefresh(mat); /* refresh */
 
	FixA0();

        /*
         * Apparently the refresh above doesn't refresh the cell whose
         * formula we just changed, and which therefore almost certainly
         * needs an update. XbaeMatrixRefreshCell also doesn't work.
         * This does :  
         */
        XbaeMatrixSetCell(mat, curow - 1, cucol - 1,
                cell_value_string(curow, cucol, True));
}

/****************************************************************
 *								*
 *			Graphs					*
 *								*
 ****************************************************************/
struct ConfigureWidgets {
	Widget	x,
		a, b, c, d, e, f, g, h, i,
		sa, sb, sc, sd, se, sf, sg, sh, si,
		ta, tb, tc, td, te, tf, tg, th, ti,
		xlog, ylog,
		title, xtitle, ytitle;
};

static void
sciplot_thunk(void *p, CELL *cp, CELLREF r, CELLREF c)
{
	char * str = (char *)char_to_q_char (print_cell (cp));

	MessageAppend(False, "SciPlotThunk(r %d c %d) %s",
		r, c, print_cell(find_cell(r, c)));
}

void UpdateGraph()
{
#ifdef	HAVE_SciPlot_H
	int			i, ii, len, xx, xa, lst;
	int			xlog, ylog;
	double			*vx, *va;
	struct rng		rngx, rnga;
	char			*s, *t;
	extern struct rng	graph_get_data(int);
	enum graph_pair_ordering order = graph_rows_hz;

	xlog = graph_get_logness('x');
	ylog = graph_get_logness('y');

	XtVaSetValues(plot,
			XtNchartType,		XtCARTESIAN,
			XtNxLabel,		graph_get_axis_title('x'),
			XtNyLabel,		graph_get_axis_title('y'),
			XtNxLog,		xlog,
			XtNyLog,		ylog,
		NULL);


	rngx = graph_get_data(0);
	rnga = graph_get_data(1);

	len = rngx.hr - rngx.lr + 1;

	vx = (double *)XtCalloc(len, sizeof(double));
	va = (double *)XtCalloc(len, sizeof(double));

	/* FIX ME */
	/* This code is wrong: only works for vertical set of data */
	for (ii=0, i=rngx.lr; i<=rngx.hr; i++, ii++) {
		s = cell_value_string(i, rngx.lc, True);
		vx[ii] = 0.0;
		sscanf(s, "%lf", &vx[ii]);
		s = cell_value_string(i, rnga.lc, True);
		va[ii] = 0.0;
		sscanf(s, "%lf", &va[ii]);
	}

/* Start test for_pairs_in() */
	for_pairs_in(&rngx, order, (fpi_thunk)sciplot_thunk, NULL);
	for_pairs_in(&rnga, order, (fpi_thunk)sciplot_thunk, NULL);
/* End test for_pairs_in() */

	lst = SciPlotListCreateDouble(plot, 10, vx, va,
			graph_get_data_title(0));
#if 1
    {
	/* Colour test */

	int	fg, bg, lc, pc;

	bg = SciPlotAllocNamedColor(plot, "seagreen");
	fg = SciPlotAllocNamedColor(plot, "yellow");
	lc = SciPlotAllocNamedColor(plot, "orange");
	pc = SciPlotAllocNamedColor(plot, "light blue");

	SciPlotListSetStyle(plot, lst, pc, XtMARKER_CIRCLE,
		lc, XtLINE_DOTTED);
	SciPlotSetForegroundColor(plot, fg);
	SciPlotSetBackgroundColor(plot, bg);
    }
#else
	SciPlotListSetStyle(plot, lst, 1, XtMARKER_CIRCLE,
		1, XtLINE_DOTTED);
#endif

	s = graph_get_title();
	if (s && strlen(s)) {
		XtVaSetValues(plot, XtNplotTitle, s, NULL);
	}

	XtFree((XtPointer)vx);
	XtFree((XtPointer)va);

	SciPlotSetXAutoScale(plot);
	SciPlotSetYAutoScale(plot);

	SciPlotUpdate(plot);
#endif
}

void CreateGraph(Widget w, XtPointer client, XtPointer call)
{
	Widget	f, sw, sep, ok;
	int	i;

	if (gs)
		return;

	gs = XtVaCreatePopupShell("graphShell",
		topLevelShellWidgetClass,
		toplevel,
	NULL);

	f = XtVaCreateManagedWidget("form", xmFormWidgetClass, gs,
		NULL);

#if	HAVE_SciPlot_H
	sw = XtVaCreateManagedWidget("scroll",
		xmScrolledWindowWidgetClass, f,
			XmNtopAttachment,	XmATTACH_FORM,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNtopOffset,		10,
			XmNleftOffset,		10,
			XmNrightOffset,		10,
		NULL);

	plot = XtVaCreateManagedWidget("plot",
		sciplotWidgetClass, sw,
			XmNmarginWidth,		20,
			XmNmarginHeight,	20,
			XmNwidth,		600,
			XmNheight,		500,
		NULL);
#else
	/* Fallback if we don't have SciPlot */
	sw = plot = NULL;
#endif

	sep = XtVaCreateManagedWidget("separator",
		xmSeparatorGadgetClass, f,
			XmNorientation,		XmHORIZONTAL,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopOffset,		10,
			XmNtopWidget,		sw,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNrightOffset,		10,
		NULL);

	ok = XtVaCreateManagedWidget("ok",
		xmPushButtonGadgetClass, f,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopOffset,		10,
			XmNtopWidget,		sep,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNrightOffset,		10,
		NULL);
	XtAddCallback(ok, XmNactivateCallback, PopDownHelpCB, gs);

	UpdateGraph();
}

/*
 * Pop up a window with a graph (from SciPlot).
 */
void ShowGraph(Widget w, XtPointer client, XtPointer call)
{
	CreateGraph(w, client, call);
	XtPopup(gs, XtGrabNone);
}

/*
 * Needs to figure out to which file/printer to print, depending on
 * the callback data passed from the dialog.
 */
void ReallyPrintGraph(Widget w, XtPointer client, XtPointer call)
{
#ifdef	HAVE_SciPlot_H
	if (gs == NULL)
		CreateGraph(w, client, call);
	else
		UpdateGraph();
	if (plot)
		SciPlotPSCreateColor(plot, "plot.ps");
#endif
}

/*
 * FIX ME
 * This must first pop up a window asking whether to print to a file
 * or a printer. Only after confirmation must ReallyPrintGraph be
 * called.
 */
void PrintGraph(Widget w, XtPointer client, XtPointer call)
{
	ReallyPrintGraph(w, client, call);
}

/*
 * Create a widget tree to configure a graph with.
 *	The result is a widget that isn't managed yet.
 *	(So you can put it in a dialog).
 *
 * Information that's present in traditional oleo graphs :
 * - general :
 *	X Axis		Title, Range
 *	Y Axis		Title, Range
 *	Logarithmic axes
 * - per data set :
 *	Data Set Name
 *	Style		(Linespoints, lines)
 */
Widget CreateConfigureGraph(Widget parent)
{
	Widget	toprc, frame, w, rc, cap;
	struct ConfigureWidgets	*cw;

	cw = (struct ConfigureWidgets *)XtMalloc(
		sizeof(struct ConfigureWidgets));

	/*
	 * We need to set the userdata on the toplevel widget here.
	 */
	toprc = XtVaCreateWidget("configureGraphRC", xmRowColumnWidgetClass,
		parent,
			XmNuserData,	cw,
		NULL);

	frame = XtVaCreateManagedWidget("dataFrame",
		xmFrameWidgetClass, toprc,
		NULL);

	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, frame,
			XmNnumColumns,	4,
			XmNorientation,	XmVERTICAL,
			XmNpacking,	XmPACK_COLUMN,
		NULL);
	XtVaCreateManagedWidget("l0", xmLabelGadgetClass, rc,
		NULL);
	XtVaCreateManagedWidget("l1", xmLabelGadgetClass, rc,
		NULL);
	XtVaCreateManagedWidget("l2", xmLabelGadgetClass, rc,
		NULL);
	XtVaCreateManagedWidget("l3", xmLabelGadgetClass, rc,
		NULL);
	XtVaCreateManagedWidget("l4", xmLabelGadgetClass, rc,
		NULL);
	XtVaCreateManagedWidget("l5", xmLabelGadgetClass, rc,
		NULL);

	XtVaCreateManagedWidget("r0", xmLabelGadgetClass, rc,
		NULL);
	cw->x = XtVaCreateManagedWidget("r1", xmTextFieldWidgetClass, rc,
		NULL);
	RegisterRangeSelector(cw->x);
	cw->a = XtVaCreateManagedWidget("r2", xmTextFieldWidgetClass, rc,
		NULL);
	RegisterRangeSelector(cw->a);
	cw->b = XtVaCreateManagedWidget("r3", xmTextFieldWidgetClass, rc,
		NULL);
	RegisterRangeSelector(cw->b);
	cw->c = XtVaCreateManagedWidget("r4", xmTextFieldWidgetClass, rc,
		NULL);
	RegisterRangeSelector(cw->c);
	cw->d = XtVaCreateManagedWidget("r5", xmTextFieldWidgetClass, rc,
		NULL);
	RegisterRangeSelector(cw->d);

	XtVaCreateManagedWidget("s0", xmLabelGadgetClass, rc,
		NULL);
	cw->sa = XtVaCreateManagedWidget("s1", xmToggleButtonGadgetClass, rc,
		NULL);
	cw->sb = XtVaCreateManagedWidget("s2", xmToggleButtonGadgetClass, rc,
		NULL);
	cw->sc = XtVaCreateManagedWidget("s3", xmToggleButtonGadgetClass, rc,
		NULL);
	cw->sd = XtVaCreateManagedWidget("s4", xmToggleButtonGadgetClass, rc,
		NULL);
	cw->se = XtVaCreateManagedWidget("s5", xmToggleButtonGadgetClass, rc,
		NULL);

	XtVaCreateManagedWidget("t0", xmLabelGadgetClass, rc,
		NULL);
	cw->ta = XtVaCreateManagedWidget("t1", xmTextFieldWidgetClass, rc,
		NULL);
	cw->tb = XtVaCreateManagedWidget("t2", xmTextFieldWidgetClass, rc,
		NULL);
	cw->tc = XtVaCreateManagedWidget("t3", xmTextFieldWidgetClass, rc,
		NULL);
	cw->td = XtVaCreateManagedWidget("t4", xmTextFieldWidgetClass, rc,
		NULL);
	cw->te = XtVaCreateManagedWidget("t5", xmTextFieldWidgetClass, rc,
		NULL);
	
	frame = XtVaCreateManagedWidget("dataFrame",
		xmFrameWidgetClass, toprc,
		NULL);
	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, frame,
		NULL);
	XtVaCreateManagedWidget("xl", xmLabelGadgetClass, rc,
		NULL);
	cw->xtitle = XtVaCreateManagedWidget("xt", xmTextFieldWidgetClass, rc,
		NULL);
	cw->xlog = XtVaCreateManagedWidget("xlog",
		xmToggleButtonGadgetClass, rc,
		NULL);
	XtVaCreateManagedWidget("yl", xmLabelGadgetClass, rc,
		NULL);
	cw->ytitle = XtVaCreateManagedWidget("yt", xmTextFieldWidgetClass, rc,
		NULL);
	cw->ylog = XtVaCreateManagedWidget("ylog",
		xmToggleButtonGadgetClass, rc,
		NULL);

	cw->title = XtVaCreateManagedWidget("title", xmTextFieldWidgetClass, rc,
		NULL);

	return toprc;
}

void ConversionError(char *s, char *t)
{
	char *r = XtMalloc(strlen(s) + strlen(t) + 100);

	sprintf(r,
		_("Conversion error: cannot convert '%s' to a %s\n"),
		s, t);
	MessageAppend(True, r);
	XtFree(r);
}

/*
 * Ok handler
 */
void ConfigureGraphOk(Widget w, XtPointer client, XtPointer call)
{
	Widget			f = (Widget)client;
	struct ConfigureWidgets	*cw = NULL;
	char			*s, *p;
	struct rng		rngx;
	int			r;

	XtVaGetValues(f, XmNuserData, &cw, NULL);

	if (cw == NULL) {
		MessageAppend(True, _("Cannot find XmNuserData\n"));
		return;
	}

	MessageAppend(False, "ConfigureGraphOk\n");

/* Plot title */
	s = XmTextFieldGetString(cw->title);
	graph_set_title(s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

/* X Axis */
	s = XmTextFieldGetString(cw->xtitle);
	graph_set_axis_title('x', s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif
	graph_set_logness('x', 1, XmToggleButtonGadgetGetState(cw->xlog));

/* Y Axis */
	s = XmTextFieldGetString(cw->ytitle);
	graph_set_axis_title('y', s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif
	graph_set_logness('y', 1, XmToggleButtonGadgetGetState(cw->ylog));

/* Data 1 */
	p = s = XmTextFieldGetString(cw->x);
	if ((r = parse_cell_or_range(&p, &rngx)) == 0)
		ConversionError(s, _("range"));
	else if (r & RANGE) {
		graph_set_data(0, &rngx, 'h', 'r');
	} else {
		rngx.hr = rngx.lr;
		rngx.hc = rngx.lc;
		graph_set_data(0, &rngx, 'h', 'r');
	}
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	s = XmTextFieldGetString(cw->tb);
	graph_set_data_title(0, s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

/* Data 2 */
	p = s = XmTextFieldGetString(cw->a);
	if ((r = parse_cell_or_range(&p, &rngx)) == 0)
		ConversionError(s, _("range"));
	else if (r & RANGE) {
		graph_set_data(1, &rngx, 'h', 'r');
	} else {
		rngx.hr = rngx.lr;
		rngx.hc = rngx.lc;
		graph_set_data(1, &rngx, 'h', 'r');
	}

#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

/* Data 3 */
	p = s = XmTextFieldGetString(cw->b);
	if ((r = parse_cell_or_range(&p, &rngx)) == 0)
		ConversionError(s, _("range"));
	else if (r & RANGE) {
		graph_set_data(2, &rngx, 'h', 'r');
	} else {
		rngx.hr = rngx.lr;
		rngx.hc = rngx.lc;
		graph_set_data(2, &rngx, 'h', 'r');
	}
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	p = s = XmTextFieldGetString(cw->c);
	if ((r = parse_cell_or_range(&p, &rngx)) == 0)
		ConversionError(s, _("range"));
	else if (r & RANGE) {
		graph_set_data(3, &rngx, 'h', 'r');
	} else {
		rngx.hr = rngx.lr;
		rngx.hc = rngx.lc;
		graph_set_data(3, &rngx, 'h', 'r');
	}
	MessageAppend(False, s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	p = s = XmTextFieldGetString(cw->d);
	if ((r = parse_cell_or_range(&p, &rngx)) == 0)
		ConversionError(s, _("range"));
	else if (r & RANGE) {
		graph_set_data(4, &rngx, 'h', 'r');
	} else {
		rngx.hr = rngx.lr;
		rngx.hc = rngx.lc;
		graph_set_data(4, &rngx, 'h', 'r');
	}
	MessageAppend(False, s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif
}

/*
 * Revert configure graph fields to their current internal values.
 *	This is needed for popping up the dialog after it's been changed by
 *	the user but he's hit Cancel.
 *	Or for values read from a file. (new to 1.6.14)
 *
 * FIX ME incomplete.
 */
void ConfigureGraphReset(Widget f)
{
	struct ConfigureWidgets	*cw = NULL;
	struct rng		r;
	char			*s;
	extern struct rng	graph_get_data(int);

	XtVaGetValues(f, XmNuserData, &cw, NULL);

	if (cw == NULL) {
		MessageAppend(True, _("Cannot find XmNuserData\n"));
		return;
	}
	MessageAppend(False, "ConfigureGraphReset\n");

	r = graph_get_data(0);
	s = range_name(&r);
	XmTextFieldSetString(cw->x, s);

	r = graph_get_data(1);
	s = range_name(&r);
	XmTextFieldSetString(cw->a, s);

	r = graph_get_data(2);
	s = range_name(&r);
	XmTextFieldSetString(cw->b, s);

	r = graph_get_data(3);
	s = range_name(&r);
	XmTextFieldSetString(cw->c, s);

	r = graph_get_data(4);
	s = range_name(&r);
	XmTextFieldSetString(cw->d, s);

	s = graph_get_title();
	XmTextFieldSetString(cw->title, s);

	s = graph_get_axis_title('x');
	XmTextFieldSetString(cw->xtitle, s);

	s = graph_get_axis_title('y');
	XmTextFieldSetString(cw->ytitle, s);
}

void ConfigureGraph(Widget w, XtPointer client, XtPointer call)
{
	Widget		ok, cancel, help;
	static Widget	configureGraph = NULL, inside = NULL;

	if (! configureGraph) {
		configureGraph = XmCreateTemplateDialog(mw, "configureGraph",
			NULL, 0);

		inside = CreateConfigureGraph(configureGraph);
		XtManageChild(inside);

		ok = XtVaCreateManagedWidget("ok", xmPushButtonGadgetClass,
			configureGraph,
			NULL);
		cancel = XtVaCreateManagedWidget("cancel",
			xmPushButtonGadgetClass, configureGraph,
			NULL);
		help = XtVaCreateManagedWidget("help", xmPushButtonGadgetClass,
			configureGraph,
			NULL);

		XtAddCallback(ok, XmNactivateCallback, ConfigureGraphOk, inside);

		/* FIX ME need something to call the help system */
	}

	ConfigureGraphReset(inside);
	XtManageChild(configureGraph);
}

/*
 * Plotutils
 */
void PuShowGraph(Widget w, XtPointer client, XtPointer call)
{
#ifdef	HAVE_LIBPLOT
	PuPieChart("X", stdout);
#endif
}

void PuPrintGraph(Widget w, XtPointer client, XtPointer call)
{
#ifdef	HAVE_LIBPLOT
	FILE	*x = fopen("x.ps", "w");
	PuPieChart("ps", x);
	fclose(x);
#endif
}

void PuShowBarChart(Widget w, XtPointer client, XtPointer call)
{
#ifdef	HAVE_LIBPLOT
	PuBarChart("X", stdout, 0);
#endif
}

void PuShowXYChart(Widget w, XtPointer client, XtPointer call)
{
#ifdef	HAVE_LIBPLOT
	PuXYChart("X", stdout, 0);
#endif
}


/*
 * Print
 */
struct PrintWidgets {
	Widget	rangeTF, fileTF, printerTF, programTF;
} PrintWidgets;

/*
 * Print the spreadsheet
 */
void ReallyPrintCB(Widget w, XtPointer client, XtPointer call)
{
	/* FIX ME
	 * This is simplistic, we need to print to either a file,
	 * selected by the user from a dialog, or to a printer,
	 * also selected by the user.
	 */
	FILE		*fp = NULL;
	struct rng	rng;
	char		*fn, *s, *p;
	int		r;

	XtUnmanageChild(PrintDialog);

	fn = XmTextFieldGetString(PrintWidgets.fileTF);

	if (fn)
		fp = fopen(fn, "w");
	if (! fp) {
		MessageAppend(True, "Couldn't write to file %s\n", fn);
#ifdef	FREE_TF_STRING
		XtFree(fn);
#endif
		return;
	}

	p = s = XmTextFieldGetString(PrintWidgets.rangeTF);
	if ((r = parse_cell_or_range(&p, &rng)) == 0)
		ConversionError(s, _("range"));
	else if (r & RANGE) {
		;
	} else {
		rng.hr = rng.lr;
		rng.hc = rng.lc;
	}
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	psprint_region_cmd(&rng, fp);
	fclose(fp);

	MessageAppend(False, "Printed %s to %s\n", range_name(&rng), fn);

#ifdef	FREE_TF_STRING
	XtFree(fn);
#endif
}

static void MotifSetPrintPage(Widget w, XtPointer client, XtPointer call)
{
	int	i = (int) client;

	MessageAppend(False, "Set page format to %s\n", PrintGetPageName(i));
	PrintSetPage(PrintGetPageName(i));
}

Widget MotifCreatePrintDialog(Widget s)
{
	Widget		form, menu, cb, w, frame, radio;
	int		npages, i, ac;
	Arg		al[5];
	XmString	xms;

	ac = 0;
	form = XmCreateForm(s, "printForm", al, ac);

	/* Destination */
	frame = XtVaCreateManagedWidget("printDestinationFrame",
			xmFrameWidgetClass, form,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNtopAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNtopOffset,		10,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNrightOffset,		10,
		NULL);

	w = XtVaCreateManagedWidget("printDestinationFrameTitle",
			xmLabelGadgetClass,		frame,
			XmNchildType,			XmFRAME_TITLE_CHILD,
			XmNchildVerticalAlignment,	XmALIGNMENT_CENTER,
		NULL);

	radio = XtVaCreateManagedWidget("printDestinationRadio",
				xmRowColumnWidgetClass, frame,
			XmNradioBehavior,	True,
			XmNradioAlwaysOne,	True,
			XmNnumColumns,		3,
			XmNorientation,		XmHORIZONTAL,
		NULL);

	w = XtVaCreateManagedWidget("printer", xmToggleButtonGadgetClass,
		radio, NULL);
	/* FIX ME - this is a hardcoded default */
	XmToggleButtonGadgetSetState(w, True, False);
	PrintWidgets.printerTF = 
	w = XtVaCreateManagedWidget("printerTF", xmTextFieldWidgetClass,
			radio, NULL);
	XmTextFieldSetString(w, AppRes.printer);
	/* Filler */ XtVaCreateManagedWidget("", xmLabelGadgetClass, radio,
		NULL);

	w = XtVaCreateManagedWidget("file", xmToggleButtonGadgetClass,
		radio, NULL);
	PrintWidgets.fileTF = 
	w = XtVaCreateManagedWidget("fileTF", xmTextFieldWidgetClass,
			radio, NULL);
	w = XtVaCreateManagedWidget("fileTFBrowse", xmPushButtonGadgetClass,
			radio, NULL);
	w = XtVaCreateManagedWidget("program", xmToggleButtonGadgetClass,
		radio, NULL);
	PrintWidgets.programTF = 
	w = XtVaCreateManagedWidget("programTF", xmTextFieldWidgetClass,
			radio, NULL);
	XmTextFieldSetString(w, AppRes.program);

	/* Paper */
	w = frame;
	frame = XtVaCreateManagedWidget("printPaperFrame",
				xmFrameWidgetClass, form,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopWidget,		w,
			XmNtopOffset,		10,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNrightOffset,		10,
		NULL);

	w = XtVaCreateManagedWidget("printPaperFrameTitle",
			xmLabelGadgetClass,		frame,
			XmNchildType,			XmFRAME_TITLE_CHILD,
			XmNchildVerticalAlignment,	XmALIGNMENT_CENTER,
		NULL);

	menu = XmCreatePulldownMenu(frame, "optionMenu", NULL, 0);
	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, menu); ac++;
	xms = XmStringCreateSimple(_("File Format"));
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	cb = XmCreateOptionMenu(frame, "optionCB", al, ac);
	XtManageChild(cb);
	XmStringFree(xms);

	npages = PrintGetNumPageSizes();

	for (i=0; i<npages; i++) {
		ac = 0;
		xms = XmStringCreateSimple(PrintGetPageName(i));
		XtSetArg(al[ac], XmNlabelString, xms); ac++;
		w = XmCreatePushButtonGadget(menu, PrintGetPageName(i), al, ac);
		if (strcmp(AppRes.paper, PrintGetPageName(i)) == 0)
			XtVaSetValues(menu, XmNmenuHistory, w, NULL);
		XtAddCallback(w, XmNactivateCallback,
			MotifSetPrintPage, (XtPointer)i);
		XmStringFree(xms);
		XtManageChild(w);
	}

	/* Range to print */
	w = frame;
	frame = XtVaCreateManagedWidget("printRangeFrame",
				xmFrameWidgetClass, form,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopWidget,		w,
			XmNtopOffset,		10,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNrightOffset,		10,
		NULL);

	w = XtVaCreateManagedWidget("printRangeFrameTitle",
			xmLabelGadgetClass,		frame,
			XmNchildType,			XmFRAME_TITLE_CHILD,
			XmNchildVerticalAlignment,	XmALIGNMENT_CENTER,
		NULL);

	PrintWidgets.rangeTF = 
	w = XtVaCreateManagedWidget("printRangeTF", xmTextFieldWidgetClass,
		frame,
		NULL);
	RegisterRangeSelector(w);

	return form;
}

void printCB(Widget w, XtPointer client, XtPointer call)
{
	Widget	d, ok, cancel, help;

	if (PrintDialog == NULL) {
		PrintDialog = XmCreateTemplateDialog(mw, "printDialog",
			NULL, 0);
		d = MotifCreatePrintDialog(PrintDialog);
		XtManageChild(d);

		ok = XtVaCreateManagedWidget("ok", xmPushButtonGadgetClass,
			PrintDialog,
			NULL);
		XtAddCallback(ok, XmNactivateCallback, ReallyPrintCB, NULL);

		cancel = XtVaCreateManagedWidget("cancel",
			xmPushButtonGadgetClass, PrintDialog, NULL);
		XtAddCallback(cancel, XmNactivateCallback,
			CancelTemplate, PrintDialog);

		help = XtVaCreateManagedWidget("help",
			xmPushButtonGadgetClass, PrintDialog, NULL);

	}

	XtManageChild(PrintDialog);
}

void SetDefaultFileCB(Widget w, XtPointer client, XtPointer call)
{
	char	*s, *xx;
	Widget	tf, f;

	f = XtParent(w);
	/* Set default file format */

	/* Set separator in List file format */
	if ((tf = XtNameToWidget(f, "*listSeparatorTF")) != NULL) {
		xx = XmTextFieldGetString(tf);
		s = XtMalloc(strlen(xx) + 8);
		strcpy(s, "list ");
		strcat(s, xx);
		list_set_options(1, s);
		XtFree(s);
	}

	XtPopdown(DefaultFileShell);
}

void CancelTemplate(Widget w, XtPointer client, XtPointer call)
{
	Widget	bb = (Widget)client;

	XtUnmanageChild(bb);
}

void CancelDialog(Widget w, XtPointer client, XtPointer call)
{
	Widget	shell = (Widget)client;

	XtPopdown(shell);
}

void DefaultFileFormatCB(Widget w, XtPointer client, XtPointer call)
{
	char		*f = (char *)client;

	file_set_default_format(f);
}

Widget CreateFileFormatOption(Widget parent, XtCallbackProc f)
{
	Arg		al[10];
	int		ac = 0;
	XmString	xms;
	Widget		cb, menu, b;

	menu = XmCreatePulldownMenu(parent, "optionMenu", NULL, 0);
	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, menu); ac++;
	xms = XmStringCreateSimple(_("File Format"));
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	cb = XmCreateOptionMenu(parent, "optionCB", al, ac);
	XtManageChild(cb);
	XmStringFree(xms);

	b = XtVaCreateManagedWidget("oleo", xmPushButtonGadgetClass,
		menu,
		NULL);
	XtAddCallback(b, XmNactivateCallback, f, "oleo");

	b = XtVaCreateManagedWidget("SYLK", xmPushButtonGadgetClass,
		menu,
		NULL);
	XtAddCallback(b, XmNactivateCallback, f, "sylk");

	b = XtVaCreateManagedWidget("SC", xmPushButtonGadgetClass,
		menu,
		NULL);
	XtAddCallback(b, XmNactivateCallback, f, "sc");

	b = XtVaCreateManagedWidget("list", xmPushButtonGadgetClass,
		menu,
		NULL);
	XtAddCallback(b, XmNactivateCallback, f, "list");

	b = XtVaCreateManagedWidget("panic", xmPushButtonGadgetClass,
		menu,
		NULL);
	XtAddCallback(b, XmNactivateCallback, f, "panic");

	return menu;
}

Widget MotifCreateDefaultFileDialog(Widget s)
{
	Widget		form, menu, cb, w, ok, cancel, frame, radio, oldframe;
	int		npages, i, ac;
	Arg		al[5];
	XmString	xms;

	ac = 0;
	form = XmCreateForm(s, "defaultFileForm", al, ac);

	/* List File Separator Character */
	frame = XtVaCreateManagedWidget("listSeparatorFrame",
			xmFrameWidgetClass, form,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNtopAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNtopOffset,		10,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNrightOffset,		10,
		NULL);

	w = XtVaCreateManagedWidget("listSeparatorFrameTitle",
			xmLabelGadgetClass,		frame,
			XmNchildType,			XmFRAME_TITLE_CHILD,
			XmNchildVerticalAlignment,	XmALIGNMENT_CENTER,
		NULL);

	w = XtVaCreateManagedWidget("listSeparatorTF", xmTextFieldWidgetClass,
			frame,
			XmNmaxLength,		1,
		NULL);

	/* Default File Format */
	oldframe = frame;
	frame = XtVaCreateManagedWidget("defaultFormatFrame",
			xmFrameWidgetClass, form,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopWidget,		oldframe,
			XmNleftOffset,		10,
			XmNtopOffset,		10,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNrightOffset,		10,
		NULL);

	w = XtVaCreateManagedWidget("defaultFormatFrameTitle",
			xmLabelGadgetClass,		frame,
			XmNchildType,			XmFRAME_TITLE_CHILD,
			XmNchildVerticalAlignment,	XmALIGNMENT_CENTER,
		NULL);

	menu = CreateFileFormatOption(frame, DefaultFileFormatCB);

	/* FIX ME : buttons */
	ok = XtVaCreateManagedWidget("ok", xmPushButtonGadgetClass, form,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopOffset,		10,
			XmNtopWidget,		frame,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNrightAttachment,	XmATTACH_NONE,
			XmNbottomAttachment,	XmATTACH_FORM,
			XmNbottomOffset,	10,
		NULL);
	XtAddCallback(ok, XmNactivateCallback, SetDefaultFileCB, NULL);

	cancel = XtVaCreateManagedWidget("cancel", xmPushButtonGadgetClass,
		form,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopOffset,		10,
			XmNtopWidget,		frame,
			XmNleftAttachment,	XmATTACH_WIDGET,
			XmNleftWidget,		ok,
			XmNleftOffset,		10,
			XmNrightAttachment,	XmATTACH_NONE,
			XmNbottomAttachment,	XmATTACH_FORM,
			XmNbottomOffset,	10,
		NULL);
	XtAddCallback(cancel, XmNactivateCallback, CancelDialog, DefaultFileShell);

	return form;
}

void DefaultFileCB(Widget w, XtPointer client, XtPointer call)
{
	if (DefaultFileShell == NULL) {
		DefaultFileShell = XtVaCreatePopupShell("defaultFileShell",
				topLevelShellWidgetClass,
				toplevel,
			NULL);
		DefaultFileDialog = MotifCreateDefaultFileDialog(DefaultFileShell);
	}

	XtManageChild(DefaultFileDialog);
	XtPopup(DefaultFileShell, XtGrabNone);
}

/****************************************************************
 *								*
 *		GUI stuff					*
 *								*
 ****************************************************************/
/*
 * XbaeMatrix calls this whenever we leave a cell.
 * The cell may have been edited, we can veto the edit by setting the
 *	doit flag to false.
 */
void LeaveCell(Widget w, XtPointer client, XtPointer call)
{
	char	*r;
	CELL	*cp;
	XbaeMatrixLeaveCellCallbackStruct *cbp =
		(XbaeMatrixLeaveCellCallbackStruct *)call;

	cbp->doit = True;

	/*
	 * Check whether this cell has a formula.
	 * Don't change the value if it does.
	 */
	if ((cp = find_cell (curow, cucol)) && cp->cell_formula) {
		Debug(__FILE__, "LeaveCell(%d,%d) has a formula !!\n",
			cbp->row, cbp->column, cbp->value);

		return;
	}

	Debug(__FILE__, "LeaveCell(%d,%d,%s)\n",
		cbp->row, cbp->column, cbp->value);

	/*
	 * Have oleo put the new cell content in place.
	 *
	 * This supposedly returns NULL when all is well, otherwise
	 * an error message.
	 */
	r = new_value(cbp->row + 1, cbp->column + 1, cbp->value);
	if (r) {
		MessageAppend(True, r);
		cbp->doit = False;	/* veto */
	} else {
		modified = 1;

		recalculate(1);
		MotifUpdateDisplay();
	}
}

/*
 * Yuck. Apparently oleo doesn't have an API to change cells.
 * I seem to have to call goto_region, *and* set curow/cucol.
 */
void EnterCell(Widget w, XtPointer client, XtPointer call)
{
	struct	rng	r;
	CELL		*cp;
	XbaeMatrixEnterCellCallbackStruct *cbp =
		(XbaeMatrixEnterCellCallbackStruct *)call;
	char		*dec;

	Debug(__FILE__, "EnterCell (%d, %d)\n", cbp->row, cbp->column);

	/* Tell oleo to move */
	curow = cbp->row + 1;
	cucol = cbp->column + 1;

	r.lr = r.hr = cbp->row + 1;
	r.hc = r.lc = cbp->column + 1;
	goto_region(&r);

	/* Change Formula editor */
#if 1
	if (cp = find_cell (curow, cucol)) {
		dec = decomp (curow, cucol, cp);
		XmTextFieldSetString(formulatf, dec);
	} else {
		XmTextFieldSetString(formulatf, "");
	}
#else
	if ((cp = find_cell (curow, cucol)) && cp->cell_formula) {
		dec = decomp (curow, cucol, cp);
		XmTextFieldSetString(formulatf, dec);
	} else {
		XmTextFieldSetString(formulatf, "");
	}
#endif
}

void ModifyVerify(Widget w, XtPointer client, XtPointer call)
{
	XbaeMatrixModifyVerifyCallbackStruct *cbp =
		(XbaeMatrixModifyVerifyCallbackStruct *)call;

	Debug(__FILE__, "ModifyVerify(%d,%d) '%s'\n",
		cbp->row, cbp->column,
		cbp->prev_text);
}

void WriteCell(Widget w, XtPointer client, XtPointer call)
{
	XbaeMatrixWriteCellCallbackStruct *cbp =
		(XbaeMatrixWriteCellCallbackStruct *)call;

	Debug(__FILE__, "WriteCell(%d, %d, %s)\n",
		cbp->row, cbp->column, cbp->string);
}

void DrawCell(Widget w, XtPointer client, XtPointer call)
{
	/* This needs to be called otherwise WriteCell won't work. */
	XbaeMatrixDrawCellCallbackStruct *cbp =
		(XbaeMatrixDrawCellCallbackStruct *)call;

#ifdef	VERBOSE
	Debug(__FILE__, "DrawCell(%d, %d, %s)\n",
		cbp->row, cbp->column, cbp->string);
#endif
	cbp->type = XbaeString;

	if (curow == cbp->row + 1 && cucol == cbp->column + 1) {
		cbp->string = cell_value_string(cbp->row + 1, cbp->column + 1,
			True);
	} else {
		cbp->string = print_cell(find_cell(cbp->row + 1,
			cbp->column + 1));
	}
}

void FormulaCB(Widget w, XtPointer client, XtPointer call)
{
	char	*s, *r;

	s = XmTextFieldGetString(w);
	Debug(__FILE__, "FormulaCB(%s) -> %d,%d\n", s, curow, cucol);

	r = new_value(curow, cucol, s);
	if (r) {
		MessageAppend(True, r);
	} else
		modified = 1;

#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	recalculate(1);		/* 1 is recalculate all */
	MotifUpdateDisplay();	/* refresh */
}

void ResizeColumnCB(Widget w, XtPointer client, XtPointer call)
{
	XbaeMatrixResizeColumnCallbackStruct *cbp =
		(XbaeMatrixResizeColumnCallbackStruct *)call;
	struct rng	rng;
	char		wid[20];

	/* Set a range which is a column */
	rng.lr = 0;
	rng.hr = -1;
	rng.lc = rng.hc = cbp->which + 1;
#if 0
	fprintf(stderr, "Resize column %d to %d, using range %s\n",
		cbp->which,
		cbp->column_widths[cbp->which],
		range_name(&rng));
#endif
	sprintf(wid, "%d", cbp->column_widths[cbp->which]);
	set_region_width(&rng, wid);
}

/****************************************************************
 *								*
 *		File interaction				*
 *								*
 ****************************************************************/
static char	fileformat[10] = { 'o', 'l', 'e', 'o', 0, 0, 0, 0, 0, 0 };
static char	pattern[13];

void MotifSetWindowName(const char *s)
{
	char	*t;

	t = XtMalloc(strlen(s) + 32);	/* enough */
	sprintf(t, _("[Oleo: %s]"), s);
	XtVaSetValues(toplevel, XmNtitle, t, NULL);
	XtVaSetValues(toplevel, XmNiconName, t, NULL);

	XtFree(t);
}

void FileFormatCB(Widget w, XtPointer client, XtPointer call)
{
	char		*f = (char *)client;
	XmString	xms;

	strcpy(fileformat, f);
	strcpy(pattern, "*.");
	strcat(pattern, f);

	xms = XmStringCreateSimple(pattern);
	XtVaSetValues(fsd, XmNpattern, xms, NULL);
	XmStringFree(xms);
}

void ReallyLoadCB(Widget w, XtPointer client, XtPointer call)
{
	XmFileSelectionBoxCallbackStruct *cbp =
		(XmFileSelectionBoxCallbackStruct *)call;
	FILE	*fp;
	char	*s;

	if (! XmStringGetLtoR(cbp->value, XmFONTLIST_DEFAULT_TAG, &s)) {
		/* handle error */
		return;
	}

	if (modified) {
		/* Handle unsaved changes */
	}

	/* From here it's irreversible */

	ResetColumnWidths();
	XbaeMatrixCancelEdit(mat, True);

	fp = fopen(s, "r");
	if (fp == NULL) {
		/* handle error */
		XtFree(s);
		return;
	}

	file_set_current(s);

	read_file_generic(fp, 0, fileformat);

	if (fclose(fp) != 0) {
		/* handle error */
		XtFree(s);
		return;
	}
	FixA0();

	/* Force calculate */
	recalculate(1);		/* 1 is all */

	/* Force redisplay */
	MotifUpdateDisplay();

	/* Set the widget as well as the spreadsheet to a default state. */
	modified = 0;
	curow = cucol = 1;
	XbaeMatrixEditCell(mat, 0, 0);

	MotifSetWindowName(s);
	MessageAppend(False, _("Read file '%s'\n"), s);

	XtFree(s);
}

void CreateFSD()
{
	Arg		al[10];
	int		ac = 0;
	XmString	xms;
	Widget		cb, menu, b;

	ac = 0;
	XtSetArg(al[ac], XmNautoUnmanage, True); ac++;
	xms = XmStringCreateSimple(_("*.oleo"));
	XtSetArg(al[ac], XmNpattern, xms); ac++;

	fsd = XmCreateFileSelectionDialog(toplevel, "selectfile",
		al, ac);
	XtDestroyWidget(
		XmFileSelectionBoxGetChild(fsd, XmDIALOG_HELP_BUTTON));
	XmStringFree(xms);

	/* Option menu */
	menu = CreateFileFormatOption(fsd, FileFormatCB);
}

void LoadCB(Widget w, XtPointer client, XtPointer call)
{
	Arg	al[10];
	int	ac = 0;

	if (fsd == NULL)
		CreateFSD();

	XtRemoveAllCallbacks(fsd, XmNokCallback);
	XtAddCallback(fsd, XmNokCallback, ReallyLoadCB, NULL);

	ac = 0;
	XtSetArg(al[ac], XmNtitle, _("Choose a file to load")); ac++;
	XtSetValues(fsd, al, ac);
	XtManageChild(fsd);
}

FILE *OleoOpenForWrite(const char *s)
{
	FILE	*fp;
	int	len;
	char	*fn;

	if (access(s, R_OK) == 0) {
		/* Maybe replace it ? */
		len = strlen(s) + 2 + strlen(".old");
		fn = malloc(len);
		strcpy(fn, s);
		strcat(fn, ".old");
		unlink(fn);
		if (rename(s, fn) < 0) {
			MessageAppend(True,
				_("OleoOpenForWrite: can't move %s to %s"),
				s, fn);
			free(fn);
			return NULL;
		}
		free(fn);
	}

	fp = fopen(s, "w");
	if (fp == NULL) {
		/* handle error */
		MessageAppend(True,
			_("OleoOpenForWrite: couldn't open file '%s' for write"),
			s);
		return NULL;
	}

	return fp;
}

void ReallySave(const char *s)
{
	FILE	*fp;

	fp = OleoOpenForWrite(s);
	if (fp == NULL) {
		/* handle error */
		MessageAppend(True,
			_("ReallySaveCB(%s): couldn't open file for writing"),
			s);
		return;
	}

	write_file_generic(fp, 0, fileformat);

	if (fclose(fp) != 0) {
		/* handle error */
		MessageAppend(True, _("ReallySaveCB: file close failed"));
		return;
	}

	modified = 0;
	MessageAppend(False, _("Saved file '%s'\n"), s);
	MotifSetWindowName(s);
}

void ReallySaveCB(Widget w, XtPointer client, XtPointer call)
{
	XmFileSelectionBoxCallbackStruct *cbp =
		(XmFileSelectionBoxCallbackStruct *)call;
	char	*s, *t;

	if (! XmStringGetLtoR(cbp->value, XmFONTLIST_DEFAULT_TAG, &s)) {
		/* handle error */
		MessageAppend(True,
		    _("ReallySaveCB: couldn't figure out file to save into"));
		return;
	}

	ReallySave(s);
	XtFree(s);
}

/*
 * Called from the menu system.
 * Always pop up a FileSB.
 */
void SaveAsCB(Widget w, XtPointer client, XtPointer call)
{
	Arg	al[10];
	int	ac = 0;

	if (fsd == NULL)
		CreateFSD();

	XtRemoveAllCallbacks(fsd, XmNokCallback);
	XtAddCallback(fsd, XmNokCallback, ReallySaveCB, NULL);

	ac = 0;
	XtSetArg(al[ac], XmNtitle, _("Choose a file to save in")); ac++;
	XtSetValues(fsd, al, ac);
	XtManageChild(fsd);
}

/*
 * Called from the menu system.
 * If we don't have a current file, call SaveAs.
 */
void SaveCB(Widget w, XtPointer client, XtPointer call)
{
	char	*s;
	if (s = file_get_current()) {
		ReallySave(s);
		return;
	}

	SaveAsCB(w, client, call);
}

/****************************************************************
 *								*
 *		Build Help Widgets				*
 *								*
 ****************************************************************/
#if	HAVE_XmHTML_H
static void
anchorCB(Widget widget, XtPointer client, XtPointer call)
{
        XmHTMLAnchorCallbackStruct *cbs = (XmHTMLAnchorCallbackStruct *) call;
	char	*anchor;

	fprintf(stderr, "AnchorCB(%s)\n", cbs->href);

        cbs->doit = True;
        cbs->visited = True;

	anchor = strchr(cbs->href, '#');

	XmHTMLAnchorScrollToName(html, anchor);
}

static void
HelpBuildWindow(void)
{
	Widget	f, sep, ok;

	if (! html) {
		hd = XtVaCreatePopupShell("helpShell",
				topLevelShellWidgetClass,
				toplevel,
			NULL);

		f = XtVaCreateManagedWidget("form", xmFormWidgetClass, hd,
			NULL);

		html = XtVaCreateManagedWidget("html",
			xmHTMLWidgetClass, f,
				XmNmarginWidth,			20,
				XmNmarginHeight,		20,
				XmNwidth,			600,
				XmNheight,			500,
				XmNenableBadHTMLWarnings,	XmHTML_NONE,
				XmNscrollBarDisplayPolicy,	XmSTATIC,
				XmNtopAttachment,		XmATTACH_FORM,
				XmNleftAttachment,		XmATTACH_FORM,
				XmNrightAttachment,		XmATTACH_FORM,
				XmNleftOffset,			10,
				XmNrightOffset,			10,
				XmNtopOffset,			10,
			NULL);

		sep = XtVaCreateManagedWidget("separator",
			xmSeparatorGadgetClass, f,
				XmNorientation,		XmHORIZONTAL,
				XmNtopAttachment,	XmATTACH_WIDGET,
				XmNtopOffset,		10,
				XmNtopWidget,		html,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNleftOffset,		10,
				XmNrightOffset,		10,
			NULL);

		ok = XtVaCreateManagedWidget("ok",
			xmPushButtonGadgetClass, f,
				XmNtopAttachment,	XmATTACH_WIDGET,
				XmNtopOffset,		10,
				XmNtopWidget,		sep,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNleftOffset,		10,
				XmNrightOffset,		10,
			NULL);
		XtAddCallback(ok, XmNactivateCallback, PopDownHelpCB, hd);

		XtAddCallback(html, XmNactivateCallback,
			(XtCallbackProc)anchorCB, NULL);
	}
}
#endif

/*
 * Load a file into the help window.
 * This probably needs to look at the language.
 */
#define	HELP_FILE_LENGTH	200000

#ifndef	HTMLDIR
#define	HTMLDIR			"/usr/local/share/oleo"
#endif

static void
HelpLoadFile(char *fn, char *anchor)
{
#if	HAVE_XmHTML_H
	char	*buffer = XtMalloc(HELP_FILE_LENGTH);
	FILE	*fp = fopen("oleo.html", "r");
	char	*n;

	if (fn == NULL) {
		XmHTMLTextSetString(html,
			_("<html><body>"
			  "Can't find the requested help file\n"
			  "</body></html>"));
		return;
	}

	n = XtMalloc(256);
	fp = fopen(fn, "r");
	if (fp == NULL) {
		sprintf(n, "%s/%s/%s",
			HTMLDIR, getenv("LANGUAGE"), fn);
		fp = fopen(n, "r");
	}

	if (fp == NULL) {
		sprintf(n, "%s/%s/%s",
			HTMLDIR, getenv("LANG"), fn);
		fp = fopen(n, "r");
	}

	if (fp == NULL) {
		sprintf(n, "%s/%s", HTMLDIR, fn);
		fp = fopen(n, "r");
	}

	if (fp) {
		fread(buffer, 1, HELP_FILE_LENGTH, fp);
		fclose(fp);
		XmHTMLTextSetString(html, buffer);
	} else {
		XmHTMLTextSetString(html,
			_("<html><body>"
			  "Can't find the requested help file\n"
			  "</body></html>"));
                return; 
	}
	XtFree(buffer);

	if (anchor)
		MessageAppend(False, _("Help requested on '%s'"), anchor);
	else
		MessageAppend(False, _("Help requested on using Oleo"));

	if (anchor)
		XmHTMLAnchorScrollToName(html, anchor);
	XtFree(n);
#endif
}

void helpUsingCB(Widget w, XtPointer client, XtPointer call)
{
#if	HAVE_XmHTML_H
	HelpBuildWindow();
	HelpLoadFile("oleo.html", client);
	XtPopup(hd, XtGrabNone);
#endif
}

void helpAboutCB(Widget w, XtPointer client, XtPointer call)
{
	/* FIX ME */	versionCB(w, client, call);
}

void helpVersionCB(Widget w, XtPointer client, XtPointer call)
{
	/* FIX ME */	versionCB(w, client, call);
}

/*
 * Row and column labels
 */
char	**rowlabels = NULL, **columnlabels = NULL, **columnmaxlengths = NULL;
short	*columnwidths = NULL;

void
MotifUpdateWidth(int col, int wid)
{
	if (! columnwidths)
		ResetColumnWidths();
	columnwidths[col - 1] = wid;

	if (mat)
		XtVaSetValues(mat,
				XmNcolumnWidths,	columnwidths,
			NULL);
}

void
ResetColumnWidths(void)
{
	int	i;

	columnwidths = (short *)XtCalloc(AppRes.columns, sizeof(short));
	for (i=0; i<AppRes.columns; i++)
		columnwidths[i] = AppRes.columnWidth;
}

void
SetRowColumnLabels(void)
{
	int	i;
	char	tmp[10];

	rowlabels = (char **)XtCalloc(AppRes.rows, sizeof(char *));
	columnlabels = (char **)XtCalloc(AppRes.columns, sizeof(char *));
	columnmaxlengths = (char **)XtCalloc(AppRes.columns, sizeof(char *));

	for (i=0; i<AppRes.rows; i++) {
		sprintf(tmp, a0 ? "%d" : "R%d", i + 1);
		rowlabels[i] = XtNewString(tmp);
	}
	for (i=0; i<AppRes.columns; i++) {
		if (a0)
			columnlabels[i] = XtNewString(col_to_str(i+1));
		else {
			sprintf(tmp, "C%d", i + 1);
			columnlabels[i] = XtNewString(tmp);
		}
		columnmaxlengths[i] = "64000";		/* ??? */
	}
}

void
ChangeRowColumnLabels(void)
{
	int	i;
	char	tmp[10];

	for (i=0; i<AppRes.rows; i++) {
		sprintf(tmp, a0 ? "%d" : "R%d", i + 1);
		XtFree(rowlabels[i]);
		rowlabels[i] = XtNewString(tmp);
	}
	for (i=0; i<AppRes.columns; i++) {
		XtFree(columnlabels[i]);
		if (a0)
			columnlabels[i] = XtNewString(col_to_str(i+1));
		else {
			sprintf(tmp, "C%d", i + 1);
			columnlabels[i] = XtNewString(tmp);
		}
		columnmaxlengths[i] = "64000";		/* ??? */
	}
}

/****************************************************************
 *								*
 *		Some option settings				*
 *								*
 ****************************************************************/
static void FixA0()
{
	static int	havea0 = -1;

	if (havea0 == a0)
		return;

	havea0 = a0;

	/* Update matrix */
	ChangeRowColumnLabels();
	XtVaSetValues(mat,
			XmNrowLabels,		rowlabels,
			XmNcolumnLabels,	columnlabels,
		NULL);
}

void ToggleA0(Widget w, XtPointer client, XtPointer call)
{
	if (a0)
		a0 = 0;
	else
		a0 = 1;

	FixA0();
}

/****************************************************************
 *								*
 *		Edit Menu					*
 *								*
 ****************************************************************/
void UndoCB(Widget w, XtPointer client, XtPointer call)
{
	none(w, client, call);
}

void EditInsertCB(Widget w, XtPointer client, XtPointer call)
{
	/* FIX ME */
		/* Need to figure out whether to insert row or column */
		/* Need to figure out how many rows/columns to treat */
	insert_row(1);
		/* Also need to clear the current cell */
	MotifUpdateDisplay();
}

void EditDeleteCB(Widget w, XtPointer client, XtPointer call)
{
	/* FIX ME */
		/* Need to figure out whether to delete row or column */
		/* Need to figure out how many rows/columns to treat */
	delete_row(1);
		/* Also need to clear the current cell */
	MotifUpdateDisplay();
}

void EditRecalculateCB(Widget w, XtPointer client, XtPointer call)
{
	recalculate(1);
	MotifUpdateDisplay();
}

/****************************************************************
 *								*
 *		Copy/Paste stuff				*
 *								*
 ****************************************************************/
/*
 * This, and the other copy/paste functions, must act as follows :
 * - a region is assumed to be selected in the spreadsheet
 * - the region is converted into more than one selections (only on
 *	request though) : text, value/formula group, ...
 * - copying to/from the X selection mechanism
 * - conversion in the other way (preferably the value/formula group,
 *	otherwise text, which can come from other sources as well... )
 *
 * Note that this bypasses the existing oleo copy/paste mechanism.
 */
void CopyCB(Widget w, XtPointer client, XtPointer call)
{
	none(w, client, call);
}

void CutCB(Widget w, XtPointer client, XtPointer call)
{
	none(w, client, call);
}

void PasteCB(Widget w, XtPointer client, XtPointer call)
{
	none(w, client, call);
}

/****************************************************************
 *								*
 *		Oleo move/copy functionality			*
 *								*
 ****************************************************************/
static Widget	copyDialog = NULL;

void CreateCopyDialog(char *t, void (*cb)(Widget, XtPointer, XtPointer))
{
	Widget		w, rc, cap;
	Arg		al[3];
	int		ac;
	XmString	ok, cancel;

	if (copyDialog == NULL) {
		ac = 0;
		ok = XmStringCreateSimple(_("OK"));
		cancel = XmStringCreateSimple(_("Cancel"));
		XtSetArg(al[ac], XmNokLabelString, ok); ac++;
		XtSetArg(al[ac], XmNcancelLabelString, cancel); ac++;
		copyDialog = XmCreateTemplateDialog(toplevel, "copyDialog",
			al, ac);
		XmStringFree(ok);
		XmStringFree(cancel);

		rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass,
			copyDialog,
			NULL);

		cap = XtVaCreateManagedWidget("cap1", xbaeCaptionWidgetClass,
			rc,
			NULL);
		w = XtVaCreateManagedWidget("r1", xmTextFieldWidgetClass, cap,
			NULL);
		cap = XtVaCreateManagedWidget("cap2", xbaeCaptionWidgetClass,
			rc,
			NULL);
		w = XtVaCreateManagedWidget("r2", xmTextFieldWidgetClass, cap,
			NULL);
	}

	XtRemoveAllCallbacks(copyDialog, XmNokCallback);
	XtAddCallback(copyDialog, XmNokCallback, cb, NULL);

	XtVaSetValues(XtParent(copyDialog), XmNtitle, t, NULL);
}

void ReallyCopyRegionCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		ft, tt;
	char		*f, *t, *p;
	struct rng	from, to;
	int		r;

	ft = XtNameToWidget(copyDialog, "*r1");
	tt = XtNameToWidget(copyDialog, "*r2");

	f = XmTextFieldGetString(ft);
	t = XmTextFieldGetString(tt);

	MessageAppend(False, "ReallyCopyRegionCB (%s -> %s)!!\n", f, t);

	p = f;
	if ((r = parse_cell_or_range(&p, &from)) == 0)
		ConversionError(f, _("range"));
	else if ((r & RANGE) == 0) {
		/* It's a cell address, not a range */
		from.hr = from.lr;
		from.hc = from.lc;
	}

	p = t;
	if ((r = parse_cell_or_range(&p, &to)) == 0)
		ConversionError(t, _("range"));
	else if ((r & RANGE) == 0) {
		/* It's a cell address, not a range */
		to.hr = to.lr;
		to.hc = to.lc;
	}

	copy_region(&from, &to);
	modified = 1;

	recalculate(1);
	MotifUpdateDisplay();

#if 0
	XtFree(f);
	XtFree(t);
#endif
}

void ReallyMoveCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		ft, tt;
	char		*f, *t, *p;
	struct rng	from, to;
	int		r;

	ft = XtNameToWidget(copyDialog, "*r1");
	tt = XtNameToWidget(copyDialog, "*r2");

	f = XmTextFieldGetString(ft);
	t = XmTextFieldGetString(tt);

	MessageAppend(False, "ReallyMoveCB (%s -> %s)!!\n", f, t);

	p = f;
	if ((r = parse_cell_or_range(&p, &from)) == 0)
		ConversionError(f, _("range"));
	else if ((r & RANGE) == 0) {
		/* It's a cell address, not a range */
		from.hr = from.lr;
		from.hc = from.lc;
	}

	p = t;
	if ((r = parse_cell_or_range(&p, &to)) == 0)
		ConversionError(t, _("range"));
	else if ((r & RANGE) == 0) {
		/* It's a cell address, not a range */
		to.hr = to.lr;
		to.hc = to.lc;
	}

	move_region(&from, &to);
	modified = 1;

	recalculate(1);
	MotifUpdateDisplay();
#if 0
	XtFree(f);
	XtFree(t);
#endif
}

void ReallyCopyValuesCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		ft, tt;
	char		*f, *t, *p;
	struct rng	from, to;
	int		r;

	ft = XtNameToWidget(copyDialog, "*r1");
	tt = XtNameToWidget(copyDialog, "*r2");

	f = XmTextFieldGetString(ft);
	t = XmTextFieldGetString(tt);

	MessageAppend(False, "ReallyCopyValuesCB (%s -> %s)!!\n", f, t);

	p = f;
	if ((r = parse_cell_or_range(&p, &from)) == 0)
		ConversionError(f, _("range"));
	else if ((r & RANGE) == 0) {
		/* It's a cell address, not a range */
		from.hr = from.lr;
		from.hc = from.lc;
	}

	p = t;
	if ((r = parse_cell_or_range(&p, &to)) == 0)
		ConversionError(t, _("range"));
	else if ((r & RANGE) == 0) {
		/* It's a cell address, not a range */
		to.hr = to.lr;
		to.hc = to.lc;
	}

	copy_values_region(&from, &to);
	modified = 1;

	recalculate(1);
	MotifUpdateDisplay();
#if 0
	XtFree(f);
	XtFree(t);
#endif
}

void CopyRegionCB(Widget w, XtPointer client, XtPointer call)
{
	CreateCopyDialog(_("Copy a region"), ReallyCopyRegionCB);
	XtManageChild(copyDialog);
}

void CopyValuesCB(Widget w, XtPointer client, XtPointer call)
{
	CreateCopyDialog(_("Copy Values"), ReallyCopyValuesCB);
	XtManageChild(copyDialog);
}

void MoveCB(Widget w, XtPointer client, XtPointer call)
{
	CreateCopyDialog(_("Move"), ReallyMoveCB);
	XtManageChild(copyDialog);
}

/****************************************************************
 *								*
 *		Formats Dialog					*
 *								*
 ****************************************************************/
int fmt;
int formats_list[] = {
	/* 0 */		FMT_DEF,
	/* 1 */		FMT_HID,
	/* 2 */		FMT_GPH,
	/* 3 */		PRC_FLT,
	/* 4 */		FMT_GEN,
	/* 5 */		FMT_DOL,
	/* 6 */		FMT_CMA,
	/* 7 */		FMT_PCT,
	/* 8 */		FMT_DEF,
	/* 9 */		FMT_DEF,
	/* 10 */	FMT_DEF,
	/* 11 */	FMT_DEF,
#ifdef	FMT_DATE
	/* 12 */	FMT_DATE,
#else
	/* 12 */	FMT_DEF,
#endif
	/* 13 */	FMT_DEF,
	/* 14 */	FMT_DEF,
	/* 15 */	FMT_DEF,
	/* 16 */	FMT_DEF
};

/*
 * This is ugly.
 *
 * Yeah gets called whenever the user manipulates the option menu.
 * The OK callback only has to figure out what got selected last
 * (from the global variable) and do its thing.
 * A cleaner but harder solution would have to figure out which
 * button is currently selected and translate that into a format.
 */
void Yeah(Widget w, XtPointer client, XtPointer call)
{
	fmt = formats_list[(int) client];
}

int	date_format;
void MotifDateFormat(Widget w, XtPointer client, XtPointer call)
{
	date_format = (int) client;
}

void CreateFormatsDialog(Widget p)
{
	Widget		frame, f, tf, l;
	XmString	xms, x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10,
			x11, x12, x13, x14, x15;
	extern char	*date_formats[];
	Widget		menu, cb;
	Arg		al[5];
	int		ac, i;

	frame = XtVaCreateManagedWidget("formatsFrame", xmFrameWidgetClass, p,
		NULL);
	l = XtVaCreateManagedWidget("formatsFrameTitle", xmLabelGadgetClass,
		frame,
			XmNchildType,			XmFRAME_TITLE_CHILD,
			XmNchildVerticalAlignment,	XmALIGNMENT_CENTER,
		NULL);

	f = XtVaCreateManagedWidget("formatsForm", xmFormWidgetClass, frame,
		NULL);

	l = XtVaCreateManagedWidget("formatsL", xmLabelGadgetClass, f,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNtopAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNtopOffset,		10,
		NULL);

	tf = XtVaCreateManagedWidget("formatsTf", xmTextFieldWidgetClass, f,
			XmNleftAttachment,	XmATTACH_WIDGET,
			XmNleftWidget,		l,
			XmNleftOffset,		10,
			XmNtopAttachment,	XmATTACH_FORM,
			XmNtopOffset,		10,
			XmNrightAttachment,	XmATTACH_FORM,
			XmNrightOffset,		10,
		NULL);
	RegisterRangeSelector(tf);

	xms = XmStringCreateSimple(_("Formats :"));

	x0 = XmStringCreateSimple(_("Default"));	/* FMT_DEF */
	x1 = XmStringCreateSimple(_("Hidden"));		/* FMT_HID */
	x2 = XmStringCreateSimple(_("Graph"));		/* FMT_GPH */
	x3 = XmStringCreateSimple(_("Float"));		/* PRC_FLT */
	x4 = XmStringCreateSimple(_("General"));	/* FMT_GEN */
	x5 = XmStringCreateSimple(_("Dollar"));		/* FMT_DOL */
	x6 = XmStringCreateSimple(_("Comma"));		/* FMT_CMA */
	x7 = XmStringCreateSimple(_("Percent"));	/* FMT_PCT */
	x8 = XmStringCreateSimple(_("Integer"));
	x9 = XmStringCreateSimple(_("Decimal"));
	x10 = XmStringCreateSimple(_("Fixed"));
	x11 = XmStringCreateSimple(_("Exponent"));
	x12 = XmStringCreateSimple(_("Date"));		/* FMT_DATE */
	x13 = XmStringCreateSimple(_("User-1"));
	x14 = XmStringCreateSimple(_("User-2"));
	x15 = XmStringCreateSimple(_("User-3"));

	w = XmVaCreateSimpleOptionMenu(f, "formatsOption",
		xms, /* mnemonic ? */0, /* initial selection */ 0, 
		/* callback */ Yeah,
		/* Type, LabelString, Mnemonic, Accelerator, AcceleratorText */
		XmVaPUSHBUTTON, x0, 'D', NULL, NULL,
		XmVaPUSHBUTTON, x1, 'P', NULL, NULL,
		XmVaPUSHBUTTON, x2, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x3, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x4, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x5, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x6, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x7, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x8, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x9, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x10, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x11, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x12, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x13, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x14, ' ', NULL, NULL,
		XmVaPUSHBUTTON, x15, ' ', NULL, NULL,

		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		tf,
		XmNtopOffset,		10,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		10,
		NULL);
	XtManageChild(w);

	XmStringFree(xms);
	XmStringFree(x0);
	XmStringFree(x1);
	XmStringFree(x2);
	XmStringFree(x3);
	XmStringFree(x4);
	XmStringFree(x5);
	XmStringFree(x6);
	XmStringFree(x7);
	XmStringFree(x8);
	XmStringFree(x9);
	XmStringFree(x10);
	XmStringFree(x11);
	XmStringFree(x12);
	XmStringFree(x13);
	XmStringFree(x14);
	XmStringFree(x15);

	/* Precision */
	l = XtVaCreateManagedWidget("precisionL", xmLabelGadgetClass, f,
			XmNleftAttachment,	XmATTACH_FORM,
			XmNleftOffset,		10,
			XmNtopAttachment,	XmATTACH_WIDGET,
			XmNtopOffset,		10,
			XmNtopWidget,		w,
		NULL);
	tf = XtVaCreateManagedWidget("precisionTf", xmTextFieldWidgetClass, f,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		w,
		XmNtopOffset,		10,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		10,
		XmNleftAttachment,	XmATTACH_WIDGET,
		XmNleftWidget,		l,
		XmNleftOffset,		10,
		NULL);

	/* A dialog for date formats */
	menu = XmCreatePulldownMenu(f, "optionMenu", NULL, 0);
	ac = 0;
	XtSetArg(al[ac], XmNsubMenuId, menu); ac++;
	xms = XmStringCreateSimple(_("Date Format"));
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	cb = XmCreateOptionMenu(f, "optionCB", al, ac);
	XtVaSetValues(cb,
		XmNtopAttachment,	XmATTACH_WIDGET,
		XmNtopWidget,		tf,
		XmNtopOffset,		10,
		XmNrightAttachment,	XmATTACH_FORM,
		XmNrightOffset,		10,
		XmNleftAttachment,	XmATTACH_FORM,
		XmNleftOffset,		10,
		XmNbottomAttachment,	XmATTACH_FORM,
		XmNbottomOffset,	10,
		NULL);
	XtManageChild(cb);
	XmStringFree(xms);

	for (i=0; date_formats[i]; i++) {
		ac = 0;
		xms = XmStringCreateSimple(date_formats[i]);
		XtSetArg(al[ac], XmNlabelString, xms); ac++;
		w = XmCreatePushButtonGadget(menu, "button", al, ac);
		if (i == 0)
			XtVaSetValues(menu, XmNmenuHistory, w, NULL);
		XtAddCallback(w, XmNactivateCallback,
			MotifDateFormat, (XtPointer)i);
		XmStringFree(xms);
		XtManageChild(w);
	}
}

void FormatsDialogOk(Widget w, XtPointer client, XtPointer call)
{
	struct rng	rng;
	Widget		ww = (Widget)client, om, b = NULL, tf, pr;
	char		*p, *s = NULL, *prec = NULL;
	int		r, precision;

	om = XtNameToWidget(ww, "*formatsFrame*formatsOption*formatsOption");
	tf = XtNameToWidget(ww, "*formatsFrame*formatsTf");
	pr = XtNameToWidget(ww, "*formatsFrame*precisionTf");

	if (om)
		XtVaGetValues(om, XmNmenuHistory, &b, NULL);
	if (tf)
		p = s = XmTextFieldGetString(tf);
	if (pr) {
		prec = XmTextFieldGetString(pr);
		if (prec) precision = atoi(prec);
	}

	/* date */
	if (fmt == FMT_DATE)
		precision = date_format;

	if (p) {
		if ((r = parse_cell_or_range(&p, &rng)) == 0)
			ConversionError(s, _("range"));
		else if (r & RANGE) {
			MessageAppend(False, "FormatRegion %s\n",
				range_name(&rng));

#if 0
		/* FIX ME
			this is an attempt to set the precision on
			a cell, it seems to have nasty side effects
		*/
			if (prec) {
				int f;
				f = 0xF0 & fmt;
				f |= precision;
				format_region(&rng, f, -1);
			} else
				format_region(&rng, fmt, -1);
#else
			format_region(&rng, fmt, -1);
#endif
			precision_region(&rng, precision);
			recalculate(1);
			MotifUpdateDisplay();
		} else {
			rng.hr = rng.lr;
			rng.hc = rng.lc;

#if 0
		/* FIX ME
			this is an attempt to set the precision on
			a cell, it seems to have nasty side effects
		*/
			if (prec) {
				int f;
				f = 0xF0 & fmt;
				f |= precision;
				format_region(&rng, f, -1);
			} else
				format_region(&rng, fmt, -1);
#else
			format_region(&rng, fmt, -1);
#endif
			precision_region(&rng, precision);
			recalculate(1);
			MotifUpdateDisplay();
		}
	}

#ifdef	FREE_TF_STRING
	XtFree(s);
	XtFree(prec);
#endif
}

void FormatsDialogReset(Widget d)
{
}

void FormatsDialog(Widget w, XtPointer Client, XtPointer call)
{
	Widget	ok, cancel, help;

	if (! FormatD) {
		FormatD = XmCreateTemplateDialog(mw, "formatsDialog",
			NULL, 0);
		CreateFormatsDialog(FormatD);

		ok = XtVaCreateManagedWidget("ok", xmPushButtonGadgetClass,
			FormatD,
			NULL);
		cancel = XtVaCreateManagedWidget("cancel",
			xmPushButtonGadgetClass, FormatD,
			NULL);
		help = XtVaCreateManagedWidget("help", xmPushButtonGadgetClass,
			FormatD,
			NULL);

		XtAddCallback(ok, XmNactivateCallback, FormatsDialogOk,
			(XtPointer)FormatD);
		XtAddCallback(help, XmNactivateCallback, helpUsingCB,
			(XtPointer)"#HelpFormats");
	}

	FormatsDialogReset(FormatD);
	XtManageChild(FormatD);
}

/****************************************************************
 *								*
 *		Build Motif GUI					*
 *								*
 ****************************************************************/
/*
 * Build a splash screen
 */

Widget
GscBuildSplash(Widget parent)
{
	Arg		al[10];
	int		ac;
	Pixmap		pm = (Pixmap)0;
	Pixel		bg, fg;
	Display		*dpy = XtDisplay(parent);
	int		depth;
	Widget		sh, rc;
	XmString	x1, x2, xms;

	sh = XtVaCreatePopupShell("splashShell", xmMenuShellWidgetClass, parent,
#if 0
			XmNallowShellResize,	False,
#endif
			XmNx,			300,
			XmNy,			300,
		NULL);

	/* MenuShell wants an RC child */
	rc = XtVaCreateManagedWidget("junk", xmRowColumnWidgetClass, sh, NULL);

	ac = 0;
	XtSetArg(al[ac], XmNshadowThickness, 0); ac++;
	splash = XmCreateLabel(rc, "splash", al, ac);

	XtVaGetValues(splash,
			XmNforeground,	&fg,
			XmNbackground,	&bg,
			XmNdepth,	&depth,
		NULL);

	ac = 0;

#if	HAVE_XPM
	pm = XCreatePixmapFromBitmapData(dpy,
		RootWindowOfScreen(XtScreen(parent)),
		(char *)splash_bits, splash_width, splash_height,
		fg, bg, depth);
	XtSetArg(al[ac], XmNlabelPixmap, pm); ac++;
	XtSetArg(al[ac], XmNlabelType, XmPIXMAP); ac++;
	XtSetValues(splash, al, ac);
#else
	x1 = XmStringCreateLtoR(GNU_PACKAGE " " VERSION "\n",
		"large");
	x2 = XmStringCreateLtoR(CopyRightString, "small");
	xms = XmStringConcat(x1, x2);
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	XtSetArg(al[ac], XmNlabelType, XmSTRING); ac++;
	XtSetValues(splash, al, ac);
	XmStringFree(x1);
	XmStringFree(x2);
	XmStringFree(xms);
#endif
	XtManageChild(splash);

	return sh;
}

/*
 * Create the main widget tree.
 */
Widget
GscBuildMainWindow(Widget parent)
{
	int	i;

	/*
	 * Structure
	 */
	mw = XmCreateMainWindow(parent, "mw", NULL, 0);
	XtManageChild(mw);

	mb = XmCreateMenuBar(mw, "menubar", NULL, 0);
	XtManageChild(mb);

	filemenu = XmCreatePulldownMenu(mb, "filemenu", NULL, 0);
	XtManageChild(filemenu);

	XtVaCreateManagedWidget("filecascade", xmCascadeButtonGadgetClass, mb,
			XmNsubMenuId,	filemenu,
		NULL);

	editmenu = XmCreatePulldownMenu(mb, "editmenu", NULL, 0);
	XtManageChild(editmenu);

	XtVaCreateManagedWidget("editcascade", xmCascadeButtonGadgetClass, mb,
			XmNsubMenuId,	editmenu,
		NULL);

	stylemenu = XmCreatePulldownMenu(mb, "stylemenu", NULL, 0);
	XtManageChild(stylemenu);

	XtVaCreateManagedWidget("stylecascade", xmCascadeButtonGadgetClass, mb,
			XmNsubMenuId,	stylemenu,
		NULL);

	optionsmenu = XmCreatePulldownMenu(mb, "optionsmenu", NULL, 0);
	XtManageChild(optionsmenu);

	XtVaCreateManagedWidget("optionscascade", xmCascadeButtonGadgetClass, mb,
			XmNsubMenuId,	optionsmenu,
		NULL);

	graphmenu = XmCreatePulldownMenu(mb, "graphmenu", NULL, 0);
	XtManageChild(graphmenu);

	XtVaCreateManagedWidget("graphcascade", xmCascadeButtonGadgetClass, mb,
			XmNsubMenuId,	graphmenu,
		NULL);

	helpmenu = XmCreatePulldownMenu(mb, "helpmenu", NULL, 0);
	XtManageChild(helpmenu);

	w = XtVaCreateManagedWidget("helpcascade", xmCascadeButtonGadgetClass,
		mb,
			XmNsubMenuId,	helpmenu,
		NULL);
	XtVaSetValues(mb, XmNmenuHelpWidget, w, NULL);

	/*
	 * Row- and column labels
	 */
	if (! columnwidths)
		ResetColumnWidths();
	SetRowColumnLabels();

	/*
	 * The main thing : BaeMatrix
	 *
	 * A couple of application resources determine how big the matrix
	 * should be made.
	 * FIX ME.
	 */
	mat = XtVaCreateManagedWidget("bae", xbaeMatrixWidgetClass, mw,
			XmNrowLabels,		rowlabels,
			XmNcolumnLabels,	columnlabels,
			XmNcolumnWidths,	columnwidths,
			XmNrows,		AppRes.rows,
			XmNcolumns,		AppRes.columns,
		NULL);
	XtAddCallback(mat, XmNleaveCellCallback, LeaveCell, NULL);
	XtAddCallback(mat, XmNenterCellCallback, EnterCell, NULL);
	XtAddCallback(mat, XmNmodifyVerifyCallback, ModifyVerify, NULL);
	XtAddCallback(mat, XmNdrawCellCallback, DrawCell, NULL);
	XtAddCallback(mat, XmNwriteCellCallback, WriteCell, NULL);
	XtAddCallback(mat, XmNselectCellCallback, SelectCellCB, NULL);
	XtAddCallback(mat, XmNresizeColumnCallback, ResizeColumnCB, NULL);

	/*
	 * We're building a small combo of two widgets which will represent
	 * one area in the MainWindow : a status area, and a formula editor.
	 */
	w = XtVaCreateManagedWidget("statusformula", xmFormWidgetClass,
		mw,
		NULL);
	statustf = XtVaCreateManagedWidget("status", xmTextFieldWidgetClass,
		w,
			XmNeditable,		False,
			XmNblinkRate,		0,
		NULL);

	formulatf = XtVaCreateManagedWidget("formula", xmTextFieldWidgetClass,
		w,
			XmNleftWidget,		statustf,
			XmNleftOffset,		5,
			XmNleftAttachment,	XmATTACH_WIDGET,
		NULL);
	XtAddCallback(formulatf, XmNactivateCallback, FormulaCB, 0);

	/*
	 * Oleo error messages
	 */
	msgtext = XmCreateScrolledText(mw, "messages", NULL, 0);
	XtVaSetValues(mw,
			XmNmessageWindow,	XtParent(msgtext),
			XmNcommandWindow,	w,
		NULL);
	XtManageChild(msgtext);


	/*
	 * Menu system contents :
	 *	File Menu.
	 */
	w = XtVaCreateManagedWidget("open", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, LoadCB, NULL);

	w = XtVaCreateManagedWidget("save", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, SaveCB, NULL);

	w = XtVaCreateManagedWidget("saveas", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, SaveAsCB, NULL);

	w = XtVaCreateManagedWidget("close", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("print", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, printCB, NULL);

	w = XtVaCreateManagedWidget("quit", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, quitCB, NULL);

	/*
	 *	Edit Menu.
	 */
	w = XtVaCreateManagedWidget("undo", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, UndoCB, NULL);

	XtVaCreateManagedWidget("sep1", xmSeparatorGadgetClass, editmenu,
		NULL);

	w = XtVaCreateManagedWidget("copy", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, CopyCB, NULL);

	w = XtVaCreateManagedWidget("cut", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, CutCB, NULL);

	w = XtVaCreateManagedWidget("paste", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PasteCB, NULL);

	XtVaCreateManagedWidget("sep2", xmSeparatorGadgetClass, editmenu,
		NULL);

	w = XtVaCreateManagedWidget("sort", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("copyvalues", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, CopyValuesCB, NULL);

	w = XtVaCreateManagedWidget("copyregion", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, CopyRegionCB, NULL);

	w = XtVaCreateManagedWidget("move", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, MoveCB, NULL);

	XtVaCreateManagedWidget("sep3", xmSeparatorGadgetClass, editmenu,
		NULL);

	w = XtVaCreateManagedWidget("insert", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, EditInsertCB, NULL);

	w = XtVaCreateManagedWidget("delete", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, EditDeleteCB, NULL);

	XtVaCreateManagedWidget("sep4", xmSeparatorGadgetClass, editmenu,
		NULL);

	w = XtVaCreateManagedWidget("recalculate", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, EditRecalculateCB, NULL);

	XtVaCreateManagedWidget("sep5", xmSeparatorGadgetClass, editmenu,
		NULL);

	w = XtVaCreateManagedWidget("setregion", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("setmark", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("gotocell", xmPushButtonGadgetClass,
		editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);


	/*
	 *	Style Menu.
	 */
	w = XtVaCreateManagedWidget("format", xmPushButtonGadgetClass,
		stylemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("alignment", xmPushButtonGadgetClass,
		stylemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("font", xmPushButtonGadgetClass,
		stylemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	XtVaCreateManagedWidget("sep1", xmSeparatorGadgetClass, stylemenu,
		NULL);

	w = XtVaCreateManagedWidget("width", xmPushButtonGadgetClass,
		stylemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("height", xmPushButtonGadgetClass,
		stylemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	XtVaCreateManagedWidget("sep2", xmSeparatorGadgetClass, stylemenu,
		NULL);

	w = XtVaCreateManagedWidget("protection", xmPushButtonGadgetClass,
		stylemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	/*
	 *	Graph Menu.
	 */
	w = XtVaCreateManagedWidget("define", xmPushButtonGadgetClass,
		graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, ConfigureGraph, NULL);

#ifdef	HAVE_SciPlot_H
	w = XtVaCreateManagedWidget("show", xmPushButtonGadgetClass, graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, ShowGraph, NULL);

	w = XtVaCreateManagedWidget("print", xmPushButtonGadgetClass, graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PrintGraph, NULL);
#endif

#ifdef	HAVE_LIBPLOT
	/* GNU Plotutils */
	w = XtVaCreateManagedWidget("pulabel", xmLabelGadgetClass, graphmenu,
		NULL);
	w = XtVaCreateManagedWidget("pushow", xmPushButtonGadgetClass,
		graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PuShowGraph, NULL);

	w = XtVaCreateManagedWidget("puprint", xmPushButtonGadgetClass,
		graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PuPrintGraph, NULL);
	w = XtVaCreateManagedWidget("pushowbar", xmPushButtonGadgetClass,
		graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PuShowBarChart, NULL);
	w = XtVaCreateManagedWidget("pushowxy", xmPushButtonGadgetClass,
		graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PuShowXYChart, NULL);
#endif


	/*
	 *	Options Menu.
	 */
	w = XtVaCreateManagedWidget("a0", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNvalueChangedCallback, ToggleA0, NULL);
	XmToggleButtonGadgetSetState(w, a0, False);

	w = XtVaCreateManagedWidget("edges", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNvalueChangedCallback, none, NULL);
	XmToggleButtonGadgetSetState(w, cwin->flags & WIN_EDGES, False);

	w = XtVaCreateManagedWidget("autorecalc", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNvalueChangedCallback, none, NULL);
	XmToggleButtonGadgetSetState(w, auto_recalc, False);

	w = XtVaCreateManagedWidget("loadhooks", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNvalueChangedCallback, none, NULL);
	XmToggleButtonGadgetSetState(w, run_load_hooks, False);

	w = XtVaCreateManagedWidget("statusline", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XmToggleButtonGadgetSetState(w, a0, False);
	XtAddCallback(w, XmNvalueChangedCallback, none, NULL);

	w = XtVaCreateManagedWidget("backup", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XmToggleButtonGadgetSetState(w, __make_backups, False);
	XtAddCallback(w, XmNvalueChangedCallback, none, NULL);

	w = XtVaCreateManagedWidget("backupcopy", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XmToggleButtonGadgetSetState(w, __backup_by_copying, False);
	XtAddCallback(w, XmNvalueChangedCallback, none, NULL);

	XtVaCreateManagedWidget("sep1", xmSeparatorGadgetClass, optionsmenu,
		NULL);

	w = XtVaCreateManagedWidget("formats", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, FormatsDialog, NULL);

	w = XtVaCreateManagedWidget("printoptions", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("defaultfileformat",
		xmPushButtonGadgetClass, optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, DefaultFileCB, NULL);

	XtVaCreateManagedWidget("sep2", xmSeparatorGadgetClass, optionsmenu,
		NULL);

	w = XtVaCreateManagedWidget("setvariable", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("showvariable", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("listvariables", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	w = XtVaCreateManagedWidget("showoptions", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, none, NULL);

	/* FIX ME */
	w = XtVaCreateManagedWidget("loadxbase", xmPushButtonGadgetClass,
		optionsmenu,
		NULL);
#ifdef	HAVE_XBASE_H
	XtAddCallback(w, XmNactivateCallback, ReadXbaseFile, NULL);
#else
	XtSetSensitive(w, False);
#endif

	/*
	 *	Help Menu.
	 */
	w = XtVaCreateManagedWidget("about", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpAboutCB, NULL);

	w = XtVaCreateManagedWidget("version", xmPushButtonGadgetClass,
		helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpVersionCB, NULL);

	w = XtVaCreateManagedWidget("using", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB, NULL);

	XtVaCreateManagedWidget("sep1", xmSeparatorGadgetClass, helpmenu, NULL);

	w = XtVaCreateManagedWidget("math", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpMathFunctions");

	w = XtVaCreateManagedWidget("trig", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpTrigonometricFunctions");

	w = XtVaCreateManagedWidget("stats", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpStatisticalFunctions");

	w = XtVaCreateManagedWidget("bool", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpBooleanFunctions");

	w = XtVaCreateManagedWidget("string", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpStringFunctions");

	w = XtVaCreateManagedWidget("struct", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpStructuralFunctions");

	w = XtVaCreateManagedWidget("search", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpSearchFunctions");

	w = XtVaCreateManagedWidget("bus", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpBusinessFunctions");

	w = XtVaCreateManagedWidget("date", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB,
		"#HelpDateFunctions");

	XtVaCreateManagedWidget("sep2", xmSeparatorGadgetClass, helpmenu, NULL);

	w = XtVaCreateManagedWidget("expr", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB, "#HelpExpressions");

	w = XtVaCreateManagedWidget("error", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB, "#HelpErrorValues");

	w = XtVaCreateManagedWidget("format", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB, NULL);

	w = XtVaCreateManagedWidget("option", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB, "#HelpOptions");

	return mw;
}

static void
xio_redraw_input_cursor (int on)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_redraw_input_cursor\n");
#endif
}

static void
xio_cellize_cursor (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_cellize_cursor\n");
#endif
	xio_redraw_input_cursor (0);
}

static void
xio_inputize_cursor (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_inputize_cursor\n");
#endif
	xio_redraw_input_cursor (1);
}

/* This redraws the input area without recomputing anything. */
static void
xio_redraw_input (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_redraw_input\n");
#endif
}

static void
xio_fix_input (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_fix_input\n");
#endif
}

static void
xio_move_cursor (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_move_cursor\n");
#endif
}

static void
xio_erase (int len)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_erase\n");
#endif
}

static void
xio_insert (int len)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_insert\n");
#endif
}
	   
static void
xio_over (char * str, int len)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_over\n");
#endif
}

static void
xio_scan_for_input (int blockp)
{
	XEvent	ev;

	if (blockp) {
#ifdef	VERBOSE
		Debug(__FILE__, "xio_scan_for_input ...");
#endif

		while (XtAppPending(app)) {
			XtAppNextEvent(app, &ev);
			(void) XtDispatchEvent(&ev);

			if (ev.type == KeyPress || ev.type == KeyRelease) {
#ifdef	VERBOSE
				Debug(__FILE__, " done\n");
#endif
				break;
			}
		}
	} else {
#ifdef	VERBOSE
		Debug(__FILE__, "xio_scan_for_input\n");
#endif
	}
		
}

static int
xio_input_avail (void)
{
	XEvent	ev;

/*
 * FIX ME
 *
 * This sucker causes response to be bumpy.
 */
#if 1
#ifdef	VERBOSE
	Debug(__FILE__, "xio_input_avail ");
#endif

	while (XtAppPending(app)) {
		XtAppNextEvent(app, &ev);
		(void) XtDispatchEvent(&ev);

		if (ev.type == KeyPress || ev.type == KeyRelease) {
#ifdef	VERBOSE
			Debug(__FILE__, "... done\n");
#endif
			break;
		}
	}
#else
#ifdef	VERBOSE
	Debug(__FILE__, "xio_input_avail\n");
#endif
#endif

	return chars_buffered;
}

static void
xio_wait_for_input (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_wait_for_input ...\n");
#endif
}

static int
xio_read_kbd (char *buffer, int size)
{
	int amt_read = size < chars_buffered ? size : chars_buffered;
#ifdef	VERBOSE
	Debug(__FILE__, "xio_read_kbd ...\n");
#endif
	if (amt_read)
		bcopy (input_buf, (char *) buffer, amt_read);
	chars_buffered -= amt_read;
	if (chars_buffered)
		bcopy ((char *) input_buf + amt_read, (char *) input_buf, chars_buffered);
	return amt_read;
}

static void
xio_nodelay (int delayp)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_nodelay ...\n");
#endif
}

static int
xio_getch (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_getch ...\n");
#endif
	return 0;
}

static void
xio_clear_input_before (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_clear_input_before\n");
#endif
}

static void
xio_clear_input_after (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_clear_input_after\n");
#endif
}

/*
 * Copy the spreadsheet values into Xbae
 * Then do an event loop until there are no more input events.
 */
static void
xio_update_status (void)
{
	XEvent	ev;
	CELL	*cp;
	char	buf[1024];
	char	*ptr, *pos = buf, *dec;

	if (!user_status) {
		Debug(__FILE__, "xio_update_status: no user_status\n");
		return;
	}

	if (mkrow != NON_ROW) {
		struct rng r;
		*pos++ = '*';
		set_rng (&r, curow, cucol, mkrow, mkcol);
		ptr = range_name (&r);
	} else
		ptr = cell_name (curow, cucol);

	strcpy(buf, ptr);

	/* Put formula in its own widget */
	if ((cp = find_cell (curow, cucol)) && cp->cell_formula) {
		dec = decomp (curow, cucol, cp);
		if (formulatf)
			XmTextFieldSetString(formulatf, dec);
	}

	/* Value */
	ptr = cell_value_string (curow, cucol, True);
	if (ptr) {
		strcat(buf, " [");
		strcat(buf, ptr);
		strcat(buf, "]");
	}

	if (statustf)
		XmTextFieldSetString(statustf, buf);


#ifdef	VERBOSE
	Debug(__FILE__, "xio_update_status(%d,%d)...", curow, cucol);
#endif

	while (XtAppPending(app)) {
		XtAppNextEvent(app, &ev);
		(void) XtDispatchEvent(&ev);
	}

#ifdef	VERBOSE
	Debug(__FILE__, " done\n");
#endif
}

static int
xio_get_chr (char *prompt)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_get_chr(%s)\n", prompt);
#endif

	MessageAppend(True, prompt);
	return io_getch ();
}

static void
xio_pr_cell_win (struct window *win, CELLREF r, CELLREF c, CELL *cp)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_pr_cell_win\n");
#endif
}

static void
xio_repaint_win (struct window *win)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_repaint_win\n");
#endif
}


static void
xio_repaint (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_repaint\n");
#endif
}


static void
xio_hide_cell_cursor (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_hide_cell_cursor\n");
#endif
}

static void
xio_display_cell_cursor (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_display_cell_cursor\n");
#endif
}

/* Refresh the existing image. */

static void
xio_redisp (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_redisp\n");
#endif
}

extern void
xio_close_display (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_close_display\n");
#endif
	if (toplevel && XtDisplay(toplevel))
		XCloseDisplay (XtDisplay(toplevel));
	toplevel = NULL;
}

static void
xio_flush (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_flush\n");
#endif
#if 0
	XFlush (theDisplay);
#endif
}


static void
xio_bell (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_bell\n");
#endif
#if 0
	XBell (XtDisplay(toplevel), 30);
	XFlush (XtDisplay(toplevel));
#endif
}

extern void 
xio_open_display (void)
{
#ifdef	VERBOSE
	Debug(__FILE__, "xio_open_display\n");
#endif
}

void
motif_error_msg(char *fmt, ...)
{
	static char	ErrorBuffer[1024];
	va_list		ap;

	va_start(ap, fmt);
	vsprintf(ErrorBuffer, fmt, ap);
	va_end(ap);

	MessageAppend(True, ErrorBuffer);
}

int
io_col_to_input_pos (int c)
{
#if 1
  Debug(__FILE__, "io_col_to_input_pos\n");
  return 0;
#else
  struct input_view * iv = &thePort->input_view;
  char * prompt = (iv->expanded_keymap_prompt
		   ? iv->expanded_keymap_prompt
		   : (iv->prompt ? iv->prompt : ""));
  int prompt_wid = iv->prompt_metric (prompt, strlen(prompt));

  c -= prompt_wid;
  {
    int cpos;
    int max = iv->visibility_end - iv->visibility_begin + 1;
    for (cpos = 1; cpos <= max; ++cpos)
      if (iv->input_metric (iv->input_area->buf + iv->visibility_begin, cpos)
	  >= c)
	break;
    return iv->visibility_begin + cpos - 1;
  }
#endif
}

void
set_x_default_point_size (int l)
{
  /* If the display is not open (ie, we're running on a non x screen, */
  /* silently return - allows stuff in startup file to work right.PGA */
  if (!toplevel)
    return;
}

void
xio_command_loop (int i)
{
	XtAppMainLoop(app);
}

void motif_init(int *argc, char **argv)
{
	io_command_loop = xio_command_loop;
	io_open_display = xio_open_display;
	io_redisp = xio_redisp;
	io_repaint = xio_repaint;
	io_repaint_win = xio_repaint_win;
	io_close_display = xio_close_display;
	io_input_avail = xio_input_avail;
	io_scan_for_input = xio_scan_for_input;
	io_wait_for_input = xio_wait_for_input;
	io_read_kbd = xio_read_kbd;
	io_nodelay = xio_nodelay;
	io_getch = xio_getch;
	io_bell = xio_bell;
	io_get_chr = xio_get_chr;
	io_update_status = xio_update_status;
	io_fix_input = xio_fix_input;
	io_move_cursor = xio_move_cursor;
	io_erase = xio_erase;
	io_insert = xio_insert;
	io_over = xio_over;
	io_flush = xio_flush;
	io_clear_input_before = xio_clear_input_before;
	io_clear_input_after = xio_clear_input_after;
	io_pr_cell_win = xio_pr_cell_win;
	io_hide_cell_cursor = xio_hide_cell_cursor;
	io_display_cell_cursor = xio_display_cell_cursor;
	io_cellize_cursor = xio_cellize_cursor;
	io_inputize_cursor = xio_inputize_cursor;

	/*
	 * XAPPLRESDIR is a variable that can be set to determine
	 *	where to find an application resource file.
	 * If it isn't defined yet, let's define one so the odds are
	 *	good for finding our (i18n) application resources.
	 */
#ifndef	APPLRESDIR
#define	APPLRESDIR	HTMLDIR
#endif

	if (getenv("XAPPLRESDIR") == NULL) {
#if	HAVE_PUTENV
		putenv("XAPPLRESDIR=" APPLRESDIR);
#endif
	}

	/*
	 * Open a connection to the display and create a toplevel shell widget.
	 */
	toplevel = XtVaAppInitialize(&app, "Oleo", NULL, 0,
		argc, argv, fallback,
		NULL);

	/*
	 * Add converter for tearoffs.
	 */
#if XmVersion >= 1002
	XmRepTypeInstallTearOffModelConverter();
#endif

	XtVaGetApplicationResources(toplevel, &AppRes,
		resources, num_resources,
		NULL);

	SplashShell = GscBuildSplash(toplevel);
	XtPopup(SplashShell, XtGrabNone);

	/* Without this we have NULL in cwin. */
	io_init_windows(AppRes.rows, AppRes.columns, 1, 2, 1, 1, 1, 1);
}

void motif_build_gui(void)
{
	XtPopdown(SplashShell);
	w = GscBuildMainWindow(toplevel);
	XtManageChild(w);
	XtRealizeWidget(toplevel);

	if (! XmProcessTraversal(mat, XmTRAVERSE_CURRENT)) {
		fprintf(stderr, _("XmProcessTraversal failed\n"));
	}

}

void ReallyQuit(Widget w, XtPointer client, XtPointer call)
{
	kill_oleo();
}

void quitCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		md;
	Arg		al[4];
	int		ac;

	if (modified && !option_filter) {
		XmString xms = XmStringCreateLtoR(
			_("There are unsaved changes.\n"
			  "Do you want to quit anyway ?"),
			XmFONTLIST_DEFAULT_TAG);

		ac = 0;
		/* Application resource for message allow i18n */
		XtSetArg(al[ac], XmNmessageString, xms); ac++;

		md = XmCreateQuestionDialog(toplevel, "quitMB", al, ac);
		XtAddCallback(md, XmNokCallback, ReallyQuit, NULL);
		XtDestroyWidget(XmMessageBoxGetChild(md,
			XmDIALOG_HELP_BUTTON));
		XtManageChild(md);
		XmStringFree(xms);
		return;
	}

	kill_oleo();
}

static char *gpl_string =
#include "gpl.h"
;

void gplCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		x, t, p;
	Arg		al[4];
	int		ac;

	
	ac = 0;
	x = XmCreateTemplateDialog(w, "versionD", al, ac);

	ac = 0;
	XtSetArg(al[ac], XmNeditable, False); ac++;
	t = XmCreateScrolledText(x, "gpl", al, ac);
	XtManageChild(t);

	XmTextSetString(t, gpl_string);

	ac = 0;
	p = XmCreatePushButtonGadget(x, "ok", al, ac);
	XtManageChild(p);

	XtManageChild(x);
}

void versionCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		x;
	XmString	xms, xms1, xms2;
	Arg		al[2];
	int		ac;

	xms1 = XmStringCreateLtoR(GNU_PACKAGE " " VERSION,
		XmFONTLIST_DEFAULT_TAG);
	xms2 = XmStringCreateLtoR(CopyRightString, XmFONTLIST_DEFAULT_TAG);
	xms = XmStringConcat(xms1, xms2);

	ac = 0;
	XtSetArg(al[0], XmNmessageString, xms); ac++;
	x = XmCreateMessageDialog(w, "versionD", al, ac);

	XtAddCallback(x, XmNhelpCallback, gplCB, NULL);

	XmStringFree(xms);
	XmStringFree(xms1);
	XmStringFree(xms2);
	XtManageChild(x);
}

/*
 * These are necessary from io-x11.c
 */
void get_x11_args (int * argc_p, char ** argv)
{
}

char *io_x11_display_name = 0;

void x11_graphics(void)
{
}

void motif_main_loop(void)
{
	XtAppMainLoop(app);
}
