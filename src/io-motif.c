/*
 *  $Id: io-motif.c,v 1.1 1998/08/19 07:45:04 danny Exp $
 *
 *  This file is part of Oleo, a free spreadsheet.
 *
 *  Copyright (C) 1998 by Danny Backx <danny.backx@advalvas.be>.
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

static char rcsid[] = "$Id: io-motif.c,v 1.1 1998/08/19 07:45:04 danny Exp $";

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/param.h>
#include <stdarg.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/XmAll.h>
#include <Xbae/Matrix.h>
#include <Xbae/Caption.h>

#if	HAVE_SciPlot_H
#include <SciPlot/SciPlot.h>
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

#include "io-motif.h"		/* To get warnings when inconsistent */

#if	HAVE_XmHTML_H
#include <XmHTML/XmHTML.h>
#endif

XtAppContext app;
Widget	toplevel, splash, SplashShell;
Widget	mw, mat, mb, filemenu, editmenu, optionsmenu, helpmenu, graphmenu;
Widget	msgtext = NULL, statustf = NULL, formulatf = NULL;
Widget	fsd = NULL;
Widget	hd, html = NULL, gs;

static Widget	w;

/* Fallback resources, application resources */
#include "fallback.h"
#include "appres.h"

GnuSheetAppres	AppRes;

/* This is ancient history */
int	cell_font_point_size = 12;
static int block_on_getch = 1;

static char	input_buf[1024];
static int	input_buf_allocated = 1024;

static int	chars_buffered = 0;

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

void MessageAppend(char *s)
{
	XmTextPosition	pos;
	int		i;

	if (msgtext == NULL || s == NULL)
		return;
	if ((i = strlen(s)) == 0)
		return;

	pos = XmTextGetLastPosition(msgtext);
	XmTextInsert(msgtext, pos, s);

	if (s[i-1] != '\n') {
		pos = XmTextGetLastPosition(msgtext);
		XmTextInsert(msgtext, pos, "\n");
	}

	XmTextShowPosition(msgtext, XmTextGetLastPosition(msgtext));
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
 *		Generic Helper Functions			*
 *								*
 ****************************************************************/
void
PopDownHelpCB(Widget widget, XtPointer client, XtPointer call)
{
	XtPopdown((Widget)client);
}

/****************************************************************
 *								*
 *			Graphs					*
 *								*
 ****************************************************************/
/* For testing purposes ... */
void DoGraph(Widget w, XtPointer client, XtPointer call)
{
	Widget	f, sw, plot, sep, ok;
	int	i;

	gs = XtVaCreatePopupShell("graphShell",
		topLevelShellWidgetClass,
		toplevel,
	NULL);

	f = XtVaCreateManagedWidget("form", xmFormWidgetClass, gs,
		NULL);

#if	HAVE_SciPlot_H
	sw = XtVaCreateManagedWidget("scroll",
		xmScrolledWindowWidgetClass, f,
#if 0
			XmNscrollingPolicy,	XmAUTOMATIC,
#endif
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
		/* test */
			XtNplotTitle,		"Test [A:J][1:2]",
			XtNchartType,		XtCARTESIAN,
		NULL);
#else
	/* Fallback */
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

#if	HAVE_SciPlot_H
	/* use the newly created graph_get_data */
{
	int			i, ii, len, xx, xa, lst;
	double			*vx, *va;
	struct rng		rngx, rnga;
	char			*s, *t;
	extern struct rng	graph_get_data(int);

	rngx = graph_get_data(0);
	rnga = graph_get_data(1);

	len = rngx.hr - rngx.lr + 1;

	vx = (double *)XtCalloc(len, sizeof(double));
	va = (double *)XtCalloc(len, sizeof(double));

	for (ii=0, i=rngx.lr; i<=rngx.hr; i++, ii++) {
		s = cell_value_string(i, rngx.lc, True);
		vx[ii] = 0.0;
		sscanf(s, "%lf", &vx[ii]);
		s = cell_value_string(i, rnga.lc, True);
		va[ii] = 0.0;
		sscanf(s, "%lf", &va[ii]);
	}
	lst = SciPlotListCreateDouble(plot, 10, vx, va, "test");
	SciPlotListSetStyle(plot, lst, 1, XtMARKER_CIRCLE,
		1, XtLINE_DOTTED);

	t = XtMalloc(100);	/* enough */
	sprintf(t, "Plotting X in %d [%d.%d], A in %d [%d.%d]\n",
		rngx.lc, rngx.lr, rngx.hr,
		rnga.lc, rnga.lr, rnga.hr);
	MessageAppend(t);

	sprintf(t, "Graph X [%s] A [%s]\n", 
			range_name(&rngx), range_name(&rnga));
	XtVaSetValues(plot, XtNplotTitle, t, NULL);
	XtFree(t);

	XtFree((XtPointer)vx);
	XtFree((XtPointer)va);
}

	SciPlotSetXAutoScale(plot);
	SciPlotSetYAutoScale(plot);

	SciPlotUpdate(plot);
#endif

	XtPopup(gs, XtGrabNone);
}

struct ConfigureWidgets {
	Widget	x,
		a, b, c, d, e, f, g, h, i;
};

/*
 * Create a widget tree to configure a graph with.
 *	The result is a widget that isn't managed yet.
 *	(So you can put it in a dialog).
 */
Widget CreateConfigureGraph(Widget parent)
{
	Widget	frame, w, rc, cap;
	struct ConfigureWidgets	*cw;

	cw = (struct ConfigureWidgets *)XtMalloc(
		sizeof(struct ConfigureWidgets));

	frame = XtVaCreateWidget("configureGraphFrame", xmFrameWidgetClass,
		parent,
			XmNuserData,	cw,
		NULL);

	rc = XtVaCreateManagedWidget("rc", xmRowColumnWidgetClass, frame,
		NULL);

	cap = XtVaCreateManagedWidget("cap1", xbaeCaptionWidgetClass, rc,
		NULL);
	cw->x = w = XtVaCreateManagedWidget("r1", xmTextFieldWidgetClass, cap,
		NULL);

	cap = XtVaCreateManagedWidget("cap2", xbaeCaptionWidgetClass, rc,
		NULL);
	cw->a = w = XtVaCreateManagedWidget("r2", xmTextFieldWidgetClass, cap,
		NULL);

	cap = XtVaCreateManagedWidget("cap3", xbaeCaptionWidgetClass, rc,
		NULL);
	cw->b = w = XtVaCreateManagedWidget("r3", xmTextFieldWidgetClass, cap,
		NULL);

	cap = XtVaCreateManagedWidget("cap4", xbaeCaptionWidgetClass, rc,
		NULL);
	cw->c = w = XtVaCreateManagedWidget("r4", xmTextFieldWidgetClass, cap,
		NULL);

	cap = XtVaCreateManagedWidget("cap5", xbaeCaptionWidgetClass, rc,
		NULL);
	cw->d = w = XtVaCreateManagedWidget("r5", xmTextFieldWidgetClass, cap,
		NULL);

	return frame;
}

void ConversionError(char *s, char *t)
{
	char *r = XtMalloc(strlen(s) + strlen(t) + 100);

	sprintf(r, "Conversion error: cannot convert '%s' to a %s\n",
		s, t);
	MessageAppend(r);
	XtFree(r);
}

/*
 * Ok handler
 */
void ConfigureGraphOk(Widget w, XtPointer client, XtPointer call)
{
	Widget			f = (Widget)client;
	struct ConfigureWidgets	*cw = NULL;
	char			*s;
	struct rng		rngx;

	XtVaGetValues(f, XmNuserData, &cw, NULL);

	if (cw == NULL) {
		MessageAppend("Cannot find XmNuserData\n");
		return;
	}

	MessageAppend("ConfigureGraphOk\n");

	s = XmTextFieldGetString(cw->x);
	if (get_abs_rng(&s, &rngx) == 0) {	/* 0 is success */
		graph_set_data(0, &rngx, 0, 0);
		fprintf(stderr, "X %d %d %d %d\n",
			rngx.lr,
			rngx.lc,
			rngx.hr,
			rngx.hc);

	} else
		ConversionError(s, "range");

#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	s = XmTextFieldGetString(cw->a);
	if (get_abs_rng(&s, &rngx) == 0) {	/* 0 is success */
		graph_set_data(1, &rngx, 0, 0);
		fprintf(stderr, "A %d %d %d %d\n",
			rngx.lr,
			rngx.lc,
			rngx.hr,
			rngx.hc);
	} else
		ConversionError(s, "range");

#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	s = XmTextFieldGetString(cw->b);
	MessageAppend(s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	s = XmTextFieldGetString(cw->c);
	MessageAppend(s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif

	s = XmTextFieldGetString(cw->d);
	MessageAppend(s);
#ifdef	FREE_TF_STRING
	XtFree(s);
#endif
}

/*
 * Revert configure graph fields to their current internal values.
 *	This is needed for popping up the dialog after it's been changed by
 *	the user but he's hit Cancel.
 */
void ConfigureGraphReset(Widget f)
{
	struct ConfigureWidgets	*cw = NULL;
	struct rng		r;
	char			*s;
	extern struct rng	graph_get_data(int);

	XtVaGetValues(f, XmNuserData, &cw, NULL);

	if (cw == NULL) {
		MessageAppend("Cannot find XmNuserData\n");
		return;
	}
	MessageAppend("ConfigureGraphReset\n");

	r = graph_get_data(0);
	s = range_name(&r);
	XmTextFieldSetString(cw->x, s);

	r = graph_get_data(1);
	s = range_name(&r);
	XmTextFieldSetString(cw->a, s);
}

void ConfigureGraph(Widget w, XtPointer client, XtPointer call)
{
	Widget		f, ok, cancel, help;
	static Widget	configureGraph = NULL;

	if (! configureGraph) {
		configureGraph = XmCreateTemplateDialog(mw, "configureGraph",
			NULL, 0);

		f = CreateConfigureGraph(configureGraph);
		XtManageChild(f);

		ok = XtVaCreateManagedWidget("ok", xmPushButtonGadgetClass,
			configureGraph,
			NULL);
		cancel = XtVaCreateManagedWidget("cancel",
			xmPushButtonGadgetClass, configureGraph,
			NULL);
		help = XtVaCreateManagedWidget("help", xmPushButtonGadgetClass,
			configureGraph,
			NULL);

		XtAddCallback(ok, XmNactivateCallback, ConfigureGraphOk, f);

		/* FIX ME need something to call the help system */
	} else {
		f = XtNameToWidget(configureGraph, "configureGraphFrame");
	}

	ConfigureGraphReset(f);
	XtManageChild(configureGraph);
}

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
		MessageAppend(r);
		cbp->doit = False;	/* veto */
	} else {
		modified = 1;

		recalculate(1);
		XbaeMatrixRefresh(mat);
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

	/* Tell oleo to move */
	Debug(__FILE__, "EnterCell (%d, %d)\n", cbp->row, cbp->column);

	curow = cbp->row + 1;
	cucol = cbp->column + 1;

	r.lr = r.hr = cbp->row + 1;
	r.hc = r.lc = cbp->column + 1;
	goto_region(&r);

	/* Change Formula editor */
	if ((cp = find_cell (curow, cucol)) && cp->cell_formula) {
		dec = decomp (curow, cucol, cp);
		XmTextFieldSetString(formulatf, dec);
	} else {
		XmTextFieldSetString(formulatf, "");
	}
	
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
	cbp->string = cell_value_string(cbp->row + 1, cbp->column + 1, True);
}

void FormulaCB(Widget w, XtPointer client, XtPointer call)
{
	char	*s, *r;

	s = XmTextFieldGetString(w);
	Debug(__FILE__, "FormulaCB(%s) -> %d,%d\n", s, curow, cucol);

	r = new_value(curow, cucol, s);
	if (r) {
		MessageAppend(r);
	} else
		modified = 1;

	XtFree(s);

	recalculate(1);		/* 1 is recalculate all */
	XbaeMatrixRefresh(mat);	/* refresh */

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
 *		File interaction				*
 *								*
 ****************************************************************/
void ReallyLoadCB(Widget w, XtPointer client, XtPointer call)
{
	XmFileSelectionBoxCallbackStruct *cbp =
		(XmFileSelectionBoxCallbackStruct *)call;
	FILE	*fp;
	char	*s, *t;

	if (! XmStringGetLtoR(cbp->value, XmFONTLIST_DEFAULT_TAG, &s)) {
		/* handle error */
		return;
	}

	if (modified) {
		/* Handle unsaved changes */
	}

	fp = fopen(s, "r");
	if (fp == NULL) {
		/* handle error */
		XtFree(s);
		return;
	}
	oleo_read_file(fp, 0);	/* How to handle more than one format ? */
	if (fclose(fp) != 0) {
		/* handle error */
		XtFree(s);
		return;
	}

	/* Force calculate */
	recalculate(1);		/* 1 is all */

	/* Set the widget as well as the spreadsheet to a default state. */
	modified = 0;
	curow = cucol = 1;
	XbaeMatrixEditCell(mat, 0, 0);

	/* Force redisplay */
	XbaeMatrixRefresh(mat);

	/* Set window title */
	t = XtMalloc(strlen(s) + 32);	/* enough */
	sprintf(t, "[Oleo: %s]", s);
	XtVaSetValues(toplevel, XmNtitle, t, NULL);
	XtVaSetValues(toplevel, XmNiconName, t, NULL);

	sprintf(t, "Read file '%s'\n", s);
	MessageAppend(t);

	XtFree(s);
	XtFree(t);
}

void LoadCB(Widget w, XtPointer client, XtPointer call)
{
	Arg	al[10];
	int	ac = 0;

	if (fsd == NULL) {
		XmString	xms;

		ac = 0;
		XtSetArg(al[ac], XmNautoUnmanage, True); ac++;
		xms = XmStringCreateSimple("*.oleo");
		XtSetArg(al[ac], XmNpattern, xms); ac++;

		fsd = XmCreateFileSelectionDialog(toplevel, "selectfile",
			al, ac);
		XtDestroyWidget(
			XmFileSelectionBoxGetChild(fsd, XmDIALOG_HELP_BUTTON));

		XmStringFree(xms);
	}

	XtRemoveAllCallbacks(fsd, XmNokCallback);
	XtAddCallback(fsd, XmNokCallback, ReallyLoadCB, NULL);

	ac = 0;
	XtSetArg(al[ac], XmNtitle, AppRes.load_title); ac++;
	XtSetValues(fsd, al, ac);
	XtManageChild(fsd);
}

void ReallySaveCB(Widget w, XtPointer client, XtPointer call)
{
	XmFileSelectionBoxCallbackStruct *cbp =
		(XmFileSelectionBoxCallbackStruct *)call;
	FILE	*fp;
	char	*s, *t;

	if (! XmStringGetLtoR(cbp->value, XmFONTLIST_DEFAULT_TAG, &s)) {
		/* handle error */
		return;
	}

	fp = fopen(s, "w");
	if (fp == NULL) {
		/* handle error */
		return;
	}
	oleo_write_file(fp, 0);	/* How to handle more than one format ? */
	if (fclose(fp) != 0) {
		/* handle error */
		return;
	}
	modified = 0;

	t = XtMalloc(strlen(s) + 32);
	sprintf(t, "Saved file '%s'\n", s);
	MessageAppend(t);
	XtFree(t);
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

	if (fsd == NULL) {
		XmString	xms;

		ac = 0;
		XtSetArg(al[ac], XmNautoUnmanage, True); ac++;
		xms = XmStringCreateSimple("*.oleo");
		XtSetArg(al[ac], XmNpattern, "*.oleo"); ac++;

		fsd = XmCreateFileSelectionDialog(toplevel, "selectfile",
			al, ac);
		XtDestroyWidget(
			XmFileSelectionBoxGetChild(fsd, XmDIALOG_HELP_BUTTON));

		XmStringFree(xms);
	}

	XtRemoveAllCallbacks(fsd, XmNokCallback);
	XtAddCallback(fsd, XmNokCallback, ReallySaveCB, NULL);

	ac = 0;
	XtSetArg(al[ac], XmNtitle, AppRes.save_as_title); ac++;
	XtSetValues(fsd, al, ac);
	XtManageChild(fsd);
}

/*
 * Called from the menu system.
 * If we don't have a current file, call SaveAs.
 */
void SaveCB(Widget w, XtPointer client, XtPointer call)
{
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

	fprintf(stderr, "AnchorCB\n");

        cbs->doit = True;
        cbs->visited = True;
}

void helpUsingCB(Widget w, XtPointer client, XtPointer call)
{
	Widget	f, sw, sep, ok;

	if (! html) {
		hd = XtVaCreatePopupShell("helpShell",
				topLevelShellWidgetClass,
				toplevel,
			NULL);

		f = XtVaCreateManagedWidget("form", xmFormWidgetClass, hd,
			NULL);

		sw = XtVaCreateManagedWidget("scroll",
			xmScrolledWindowWidgetClass, f,
				XmNtopAttachment,	XmATTACH_FORM,
				XmNleftAttachment,	XmATTACH_FORM,
				XmNrightAttachment,	XmATTACH_FORM,
				XmNtopOffset,		10,
				XmNleftOffset,		10,
				XmNrightOffset,		10,
			NULL);

		html = XtVaCreateManagedWidget("html",
			xmHTMLWidgetClass, sw,
				XmNmarginWidth, 20,
				XmNmarginHeight, 20,
				XmNwidth, 600,
				XmNheight, 500,
			NULL);

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
		XtAddCallback(ok, XmNactivateCallback, PopDownHelpCB, hd);

#if 0
		XmHTMLTextSetString(html,
			"<html><body>"
			"There's <b><i>no</i></b> help yet."
			"</body></html>" );
#else
{
#define	LEN	200000
	char	*buffer = XtMalloc(LEN);
	FILE	*fp = fopen("oleo.html", "r");
	if (fp == NULL)
		fp = fopen("oleo_1.html", "r");
	if (fp == NULL)
		fp = fopen("oleo.html", "r");
	if (fp) {
		fread(buffer, 1, LEN, fp);
		fclose(fp);
		XmHTMLTextSetString(html, buffer);
	}
	XtFree(buffer);
}
#endif
		XtAddCallback(html, XmNactivateCallback,
			(XtCallbackProc)anchorCB, NULL);


	}

	XtPopup(hd, XtGrabNone);
}

void helpAboutCB(Widget w, XtPointer client, XtPointer call)
{
}

void helpVersionCB(Widget w, XtPointer client, XtPointer call)
{
}

#endif

/*
 * Row and column labels
 */
char	**rowlabels, **columnlabels, **columnmaxlengths;

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
void ToggleA0(Widget w, XtPointer client, XtPointer call)
{
	if (a0)
		a0 = 0;
	else
		a0 = 1;

	/* Update matrix */
	ChangeRowColumnLabels();
	XtVaSetValues(mat,
			XmNrowLabels,		rowlabels,
			XmNcolumnLabels,	columnlabels,
		NULL);
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
	XmString	xms = NULL;

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
	xms = XmStringCreateSimple(GNU_PACKAGE " " VERSION);
	XtSetArg(al[ac], XmNlabelString, xms); ac++;
	XtSetArg(al[ac], XmNlabelType, XmSTRING); ac++;
	XtSetValues(splash, al, ac);
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
			XmNrows,		AppRes.rows,
			XmNcolumns,		AppRes.columns,
		NULL);
	XtAddCallback(mat, XmNleaveCellCallback, LeaveCell, NULL);
	XtAddCallback(mat, XmNenterCellCallback, EnterCell, NULL);
	XtAddCallback(mat, XmNmodifyVerifyCallback, ModifyVerify, NULL);
	XtAddCallback(mat, XmNdrawCellCallback, DrawCell, NULL);
	XtAddCallback(mat, XmNwriteCellCallback, WriteCell, NULL);

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
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);

	w = XtVaCreateManagedWidget("quit", xmPushButtonGadgetClass, filemenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, quitCB, NULL);

	/*
	 *	Edit Menu.
	 */
	w = XtVaCreateManagedWidget("undo", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);

	w = XtVaCreateManagedWidget("copy", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);

	w = XtVaCreateManagedWidget("cut", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);

	w = XtVaCreateManagedWidget("paste", xmPushButtonGadgetClass, editmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);

	/*
	 *	Graph Menu.
	 */
	w = XtVaCreateManagedWidget("configure", xmPushButtonGadgetClass, graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, ConfigureGraph, NULL);

	w = XtVaCreateManagedWidget("Debug", xmPushButtonGadgetClass, graphmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, DoGraph, NULL);

	/*
	 *	Options Menu.
	 */
	w = XtVaCreateManagedWidget("a0", xmToggleButtonGadgetClass,
		optionsmenu,
		NULL);
	XtAddCallback(w, XmNvalueChangedCallback, ToggleA0, NULL);
	XmToggleButtonGadgetSetState(w, a0, False);

	w = XtVaCreateManagedWidget("Debug", xmPushButtonGadgetClass, optionsmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, PrintDebug, NULL);

	/*
	 *	Help Menu.
	 */
	w = XtVaCreateManagedWidget("about", xmPushButtonGadgetClass, helpmenu,
		NULL);
#if	HAVE_XmHTML_H
	XtAddCallback(w, XmNactivateCallback, helpAboutCB, NULL);
#else
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);
#endif

	w = XtVaCreateManagedWidget("version", xmPushButtonGadgetClass, helpmenu,
		NULL);
#if	HAVE_XmHTML_H
	XtAddCallback(w, XmNactivateCallback, helpVersionCB, NULL);
#else
	XtAddCallback(w, XmNactivateCallback, versionCB, NULL);
#endif


#if	HAVE_XmHTML_H
	w = XtVaCreateManagedWidget("using", xmPushButtonGadgetClass, helpmenu,
		NULL);
	XtAddCallback(w, XmNactivateCallback, helpUsingCB, NULL);
#endif

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

	MessageAppend(prompt);
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

	MessageAppend(ErrorBuffer);
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

	toplevel = XtVaAppInitialize(&app, "Oleo", NULL, 0,
		argc, argv, fallback,
		NULL);

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
		fprintf(stderr, "XmProcessTraversal failed\n");
	}

}

void ReallyQuit(Widget w, XtPointer client, XtPointer call)
{
	exit(0);
}

void quitCB(Widget w, XtPointer client, XtPointer call)
{
	Widget		md;
	Arg		al[4];
	int		ac;

	if (modified) {
		ac = 0;
		/* Application resource for message allow i18n */
		XtSetArg(al[ac], XmNmessageString, AppRes.quit_message); ac++;
		md = XmCreateQuestionDialog(w, "quitMB", al, ac);
		XtAddCallback(md, XmNokCallback, ReallyQuit, NULL);
		XtDestroyWidget(XmMessageBoxGetChild(md,
			XmDIALOG_HELP_BUTTON));
		XtManageChild(md);
		return;
	}

	exit(0);
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
	XmString	xms;
	Arg		al[2];
	int		ac;

	xms = XmStringCreateLtoR(
		GNU_PACKAGE VERSION "\n"
		"Motif interface � 1998 by Danny Backx <danny.backx@advalvas.be>\n"
		"\n"
		"Oleo is licensed by the GNU Public License (GPL)\n"
		"  see http://www.gnu.org",
		XmFONTLIST_DEFAULT_TAG
		);

	ac = 0;
	XtSetArg(al[0], XmNmessageString, xms); ac++;
	x = XmCreateMessageDialog(w, "versionD", al, ac);

	XtAddCallback(x, XmNhelpCallback, gplCB, NULL);
	XmStringFree(xms);
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