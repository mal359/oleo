/*
 * Copyright (C) 1998-2000 Free Software Foundation, Inc.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * $Id: plot.c,v 1.20 2000/07/03 16:33:02 danny Exp $
 *
 * This file contains the code to draw plots from the Oleo data
 * layered on top of the libsciplot functions.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef	WITH_DMALLOC
#include <dmalloc.h>
#endif

#include <ctype.h>
#include "global.h"
#include "graph.h"
#include "cmd.h"
#include "line.h"
#include "io-term.h"
#include "info.h"
#include "cell.h"
#include "regions.h"
#include "ref.h"
#include "io-utils.h"

#include <X11/Intrinsic.h>

void
PlotInit(void)
{
	if (! Global->PlotGlobal) {
		Global->PlotGlobal = (struct PlotGlobalType *)XtMalloc(sizeof(struct PlotGlobalType));
		memset(Global->PlotGlobal, 0, sizeof(struct PlotGlobalType));

		graph_set_axis_lo('x', "0.0");
		graph_set_axis_hi('x', "100.0");
		graph_set_axis_lo('y', "0.0");
		graph_set_axis_hi('y', "100.0");
		graph_set_axis_auto(0, 1);	/* X auto */
		graph_set_axis_auto(1, 1);	/* Y auto */
		graph_set_linetooffscreen(0);

		Global->PlotGlobal->img_width = PLOT_WINDOW_WIDTH;
		Global->PlotGlobal->img_height = PLOT_WINDOW_HEIGHT;
	}

}


/*
 * Don't compile this source unless you have GNU Plotutils
 */
#ifdef	HAVE_LIBPLOT

#include <plot.h>
#include <oleo_plot.h>

#ifdef	HAVE_LIBSCIPLOT
#include <sciplot/sciplot.h>
#else
#include <sciplot.h>
#endif

static plPlotter *handle;

static char	*defaultcolor = "black";
static char	*colors[] = { "yellow", "green", "blue", "red",
				"magenta", "beige", "orange", "pink"};
static int	ncolors = sizeof(colors) / sizeof(char *);

#ifdef	HAVE_MOTIF
static Display	*Sdpy;
static Window	window;
#endif

/*
 * Internal functions - initialize and close GNU PlotUtils
 */
static void
PuOpen(const char *plotter, FILE *outfile)
{
	int	r;
	plPlotterParams	*plotter_params;

	plotter_params = pl_newplparams();

#ifdef	HAVE_MOTIF
	/* This would crash plots into a file */
	if (strcmp(plotter, "Xdrawable") == 0) {
		pl_setplparam(plotter_params, "XDRAWABLE_DISPLAY", Sdpy);
		pl_setplparam(plotter_params, "XDRAWABLE_DRAWABLE1", &window);
	}
#endif

	handle = pl_newpl_r(plotter, NULL, outfile, stderr, plotter_params);
	r = pl_openpl_r(handle);
	pl_deleteplparams(plotter_params);

	pl_filltype_r(handle, 1);
	pl_joinmod_r(handle, "round");
	pl_flinewidth_r(handle, 2);
	pl_pencolorname_r(handle, defaultcolor);

	PlotInit();
}

void
PuX(Display *dpy, Window w)
{
#ifdef	HAVE_MOTIF
	Sdpy = dpy;
	window = w;
#endif
}

static void
PuClose()
{
	pl_deletepl_r(handle);
}

/*
 * Get a float value out of a spreadsheet cell
 */
static double float_cell(CELL *cp)
{
	char		*s;

	switch (GET_TYP (cp)) {
	case TYP_INT:
	    return (double) (cp->cell_int);
	case TYP_FLT:
	    return cp->cell_flt;
	case TYP_STR:
	    s = cp->cell_str;
	    return astof (&s);
	default:
	    return 0.;
	}
}

/*
 * Simple Pie Chart
 *
 * This function will create a pie chart on a given plotter.
 * Some plotters put their output on a file stream.
 * For this you can pass a file pointer.
 *
 * Oleo can handle an X plotter (NULL file), or e.g. a PostScript
 *	plotter (the file pointer is where the PostScript commands
 *	are sent).
 *
 * This code borrows from a piechart example by Bernhard Reiter.
 */
void
PuPieChart(char *plotter, FILE *outfile)
{
	int	i, num, c;
	char	*s, **labels;
	double	curr, incr, r;
	CELL	*cp;
	struct rng	rngx;
	Point	p;

	Multigrapher	*mg;

	PuOpen(plotter, outfile);
	mg = sp_create_plot(handle, SP_PLOT_PIE);
	sp_begin_plot(mg, 1.0, 0.0, 0.0);

	/* Title */
	s = graph_get_title();
	sp_set_title(mg, s);

	memset(&p, 0, sizeof(Point));

	/* Get the data labels */
	rngx = graph_get_data(0);
	num = 0;
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range()))
		num++;
	labels = (char **)calloc(num, sizeof(char *));

	rngx = graph_get_data(0);
	i = 0;
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range())) {
		if (GET_TYP(cp) == TYP_STR)
			labels[i] = strdup(cp->cell_str);
		else if (GET_TYP(cp) == TYP_FLT)
			labels[i] = strdup(flt_to_str(cp->cell_flt));
		else
			labels[i] = NULL;
		i++;
	}

	/*
	 * Get the data points
	 *
	 * Do note that we're picking up all ranges of data points here,
	 * even if doing so doesn't really make sense.
	 */
	incr = curr = 0.0;
	c = 0;
	for (r = 1; r < NUM_DATASETS; r++) {
		rngx = graph_get_data(r);

		if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
			continue;

		make_cells_in_range(&rngx);
		i = 0;
		while ((cp = next_cell_in_range())) {

			p.label = labels[i];
			p.x = float_cell(cp);

			sp_plot_point(mg, &p);

			i++;
		}
	}

	for (i=0; i<num; i++)
		if (labels[i])
			free(labels[i]);
	free(labels);

	sp_end_plot(mg);
	sp_destroy_plot(mg);

	PuClose();
}

/*
 * Stacked and unstacked bar charts
 */
void
PuBarChart(char *plotter, FILE *outfile)
{
	int		i, r;
	struct rng	rngx;
	CELL		*cp;
	Multigrapher	*mg;

	PuOpen(plotter, outfile);

	mg = sp_create_plot(handle, SP_PLOT_BAR);

	sp_set_title(mg, graph_get_title());
	sp_set_axis_title(mg, X_AXIS, graph_get_axis_title('x'));
	sp_set_axis_title(mg, Y_AXIS, graph_get_axis_title('y'));

	sp_begin_plot(mg, 1.0, 0.0, 0.0);

#if 0
	for (i=0; i<4; i++)
		fprintf(stderr, "Oleo data label %d - %s\n", i, graph_get_data_title(i));
#endif

	sp_first_dataset(mg);
	for (r = 1; r < NUM_DATASETS; r++) {
		rngx = graph_get_data(r);

		if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
			continue;

		sp_legend_label(mg, r, graph_get_data_title(r));

		make_cells_in_range(&rngx);
		i = 0;
		while ((cp = next_cell_in_range())) {
			sp_plot_point_simple(mg, 0, 0.0, float_cell(cp));
			i++;
		}
		sp_next_dataset(mg);
	}

	sp_legend_draw(mg);
	sp_end_plot(mg);
	PuClose();
}

/*
 * XY charts
 */
void
PuXYChart(char *plotter, FILE *outfile)
{
	int		i, r, num, nalloc;
	double		x, y, xmin, xmax, delta, *xes, ymax, ymin, oldx, oldy;
	struct rng	rngx;
	CELL		*cp;
	Multigrapher	*mg;

	PuOpen(plotter, outfile);

	/* Figure out X axis borders */
	rngx = graph_get_data(0);
	make_cells_in_range(&rngx);
	cp = next_cell_in_range();
	xmin = xmax = float_cell(cp);
	nalloc = 0;
	while ((cp = next_cell_in_range())) {
		    y = float_cell(cp);
		    nalloc++;
		    if (y < xmin)
			xmin = y;
		    else if (y > xmax)
			xmax = y;
	}

	xes = (double *)calloc(nalloc + 1, sizeof(double));
	make_cells_in_range(&rngx);
	i = 0;
	while ((cp = next_cell_in_range())) {
		if (i > nalloc)
			abort();
		xes[i++] = float_cell(cp);
	}

	if (graph_get_axis_auto(0)) {
		delta = xmax - xmin;
		xmin -= delta * 0.1;
		xmax += delta * 0.1;
	} else {
		xmin = graph_get_axis_lo(0);
		xmax = graph_get_axis_lo(1);
	}

	if (graph_get_axis_auto(1)) {
	    for (r = 1; r < NUM_DATASETS; r++) {
		rngx = graph_get_data(r);

		if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
		    continue;

		    make_cells_in_range(&rngx);
		while ((cp = next_cell_in_range())) {
		    y = float_cell(cp);
		    num++;
		    if (y < ymin)
			ymin = y;
		    else if (y > ymax)
			ymax = y;
		}
	    }
	} else {
		ymin = XYyMin;	/* FIX ME */
		ymax = XYyMax;
	}

#if 0
	fprintf(stderr, "Axis ranges - X [%f..%f], Y [%f..%f]\n",
		xmin, xmax, ymin, ymax);
#endif

	mg = sp_create_plot(handle, SP_PLOT_XY);

	sp_set_title(mg, graph_get_title());
	sp_set_axis_title(mg, X_AXIS, graph_get_axis_title('x'));
	sp_set_axis_title(mg, Y_AXIS, graph_get_axis_title('y'));

	sp_begin_plot(mg, 1.0, 0.0, 0.0);

	sp_set_axis_range(mg, X_AXIS, xmin, xmax, 0.0, 0);
	sp_set_axis_range(mg, Y_AXIS, ymin, ymax, 0.0, 0);
#if 0
	/*
	 * This is experimental code to force dates to be used for the X axis tick marks.
	 */
	sp_set_axis_ticktype_date(mg, X_AXIS,
		/* round_to */		1.0,
		/* incr */		1.0,
		/* format_string */	"%b\n%Y");
#endif

	sp_draw_frame(mg, 1);

#if 0
	for (i=0; i<4; i++)
		fprintf(stderr, "Oleo data label %d - %s\n", i, graph_get_data_title(i));
#endif

	for (r = 1; r < NUM_DATASETS; r++) {
	    rngx = graph_get_data(r);

	    if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
		continue;

#if 0
	    make_cells_in_range(&rngx);
	    while ((cp = next_cell_in_range())) {
		    y = float_cell(cp);
		    num++;
		    if (y < ymin)
			ymin = y;
		    else if (y > ymax)
			ymax = y;
	    }
#endif

	    sp_legend_label(mg, r, graph_get_data_title(r));

	    if (! XYyAuto) {
		ymin = XYyMin;	/* FIX ME */
		ymax = XYyMax;
	    }

/*	    sp_setcolor(mg, defaultcolor);	*/
	
	    make_cells_in_range(&rngx);
	    i = 0;
	    while ((cp = next_cell_in_range())) {
		    if (i > nalloc) {
			abort();
		    }
		    x = xes[i];
		    y = float_cell(cp);

		    if (i > 0) {
			double	x1, x2, y1, y2;
			int	out1 = 0, out2 = 0;

			x1 = 10.0 * (oldx - xmin) / (xmax - xmin);
			y1 = 10.0 * (oldy - ymin) / (ymax - ymin);
			x2 = 10.0 * (x - xmin) / (xmax - xmin);
			y2 = 10.0 * (y - ymin) / (ymax - ymin);

			if (x1 < 0.0 || x1 > 10.0 || y1 < 0.0 || y1 > 10.0)
				out1 = 1;
			if (x2 < 0.0 || x2 > 10.0 || y2 < 0.0 || y2 > 10.0)
				out2 = 1;

			if (Global->PlotGlobal->LineToOffscreen	/* Always draw */
					|| (out1 == 0 && out2 == 0)) {
				sp_plot_point_simple(mg, 0, oldx, oldy);
				sp_plot_point_simple(mg, 1, x, y);
			}

		    }

		    oldx = x;
		    oldy = y;

		    i++;
	    }

	    sp_next_dataset(mg);
	}

	sp_legend_draw(mg);

	sp_end_plot(mg);
	sp_destroy_plot(mg);

	PuClose();
	free((void *)xes);
}
#endif	/* HAVE_LIBPLOT */
