/*
 * Copyright (C) 1998-1999 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

/*
 * These must become static somehow - FIX ME.
 */
/*
 * This needs to be put into the graph system
 * by using graph_set_*() API's.
 */
double	XYxMin = 0.0, XYxMax = 100.0, XYyMin = 0.0, XYyMax = 100.0;
int	XYxAuto = 1, XYyAuto = 1;
int	LineToOffscreen = 0;


/*
 * Don't compile this source unless you have GNU Plotutils
 */
#ifdef	HAVE_LIBPLOT

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

#include <plot.h>

static int handle;

static char	*defaultcolor = "black";
static char	*colors[] = { "yellow", "green", "blue", "red",
				"magenta", "beige", "orange", "pink"};
static int	ncolors = sizeof(colors) / sizeof(char *);

/*
 * The API changed from version 2.2 of GNU plotutils.
 * Cope with that.
 */
#if !defined(HAVE_LIBPLOT_2_2)
#define pl_newpl		newpl
#define pl_openpl		openpl
#define pl_selectpl		selectpl
#define pl_filltype		filltype
#define pl_joinmod		joinmod
#define pl_flinewidth		flinewidth
#define pl_fline		fline
#define pl_pencolorname		pencolorname
#define pl_deletepl		deletepl
#define pl_fspace		fspace
#define pl_fmove		fmove
#define pl_fcont		fcont
#define pl_farc			farc
#define pl_endpath		endpath
#define pl_alabel		alabel
#define pl_box			box
#define pl_fbox			fbox
#define pl_fmarker		fmarker
#define	pl_fillcolorname	fillcolorname
#endif

/*
 * Internal functions - initialize and close GNU PlotUtils
 */
static void
PuOpen(const char *plotter, FILE *outfile)
{
	int	r;

	handle = pl_newpl(plotter, NULL, outfile, stderr);
	pl_selectpl(handle);
	r = pl_openpl();

	pl_filltype(1);
	pl_joinmod("round");
	pl_flinewidth(2);
	pl_pencolorname(defaultcolor);
}

static void
PuClose()
{
	pl_selectpl(0);
	pl_deletepl(handle);
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
	char	*s;
	double	total = 0., curr, incr, r = 7, f,	/* FIX ME */
		*lx, *ly, la;
	CELL	*cp;
	struct rng	rngx;


#define	X(r,a)	(cos(a)*(r))
#define	Y(r,a)	(sin(a)*(r))
#define	RAD(a)	((a)/180.*M_PI)
#define	XY(r,a)	X((r),RAD(a)),Y((r),RAD(a))

	rngx = graph_get_data(0);
	PuOpen(plotter, outfile);
	pl_fspace(-10., -10., 10., 10.);			/* FIX ME */

	/* Compute total */
	num = 0;
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range())) {
		total += float_cell(cp);
		num++;
	}

	/* Allocate space for label coordinates */
	lx = calloc(num, sizeof(double));
	if (lx == NULL)
		return;
	ly = calloc(num, sizeof(double));
	if (ly == NULL)
		return;

	incr = curr = 0.0;
	c = 0; i = 0;
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range())) {
		f = float_cell(cp);
		incr = f/total*360.;

		pl_fmove(0.0,0.0);
		pl_fillcolorname(colors[c]);
		pl_fcont(XY(r,curr));
		if (incr > 179) {
			pl_farc(0.0,0.0,XY(r,curr),XY(r,curr+179.));
			curr += 179.;
			incr -= 179;
		}
		pl_farc(0.0,0.0,XY(r,curr),XY(r,incr+curr));
		pl_fcont(0.0,0.0);
		pl_endpath();

		la = curr + incr / 2.0;
		lx[i] = X(8.0, RAD(la));
		ly[i] = Y(8.0, RAD(la));

		curr += incr;
		c = (c+1) % ncolors;
		i++;
	}

	/* Title */
	s = graph_get_title();
	pl_fmove(0.,-9.);					/* FIX ME */
	pl_alabel(1, 1, s);

	/* Place labels */
	i=0;
	rngx = graph_get_data(1);
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range())) {
		pl_fmove(lx[i], ly[i]);
		if (GET_TYP (cp) == TYP_STR) {
		    pl_alabel(1, 1, cp->cell_str);
		}
		i++;
	}

	free(lx);
	free(ly);

	PuClose();

#undef	X(r,a)
#undef	Y(r,a)
#undef	RAD(a)
#undef	XY(r,a)
}

/*
 * Stacked and unstacked bar charts
 */
void
PuBarChart(char *plotter, FILE *outfile)
{
	int		i, num, r, nsets, dsvalid[NUM_DATASETS + 1];
	double		x, y, y1, y2, ymin, ymax, *ys;
	struct rng	rngx;
	CELL		*cp;

	int		stacked = 1;				/* FIX ME only stacked for now */

	PuOpen(plotter, outfile);

	/* How many items ? */
	nsets = num = 0;
	for (r = 1; r < NUM_DATASETS; r++) {
		dsvalid[r] = 0;
		rngx = graph_get_data(r);

		if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
			continue;

		dsvalid[r] = 1;
		nsets++;

		make_cells_in_range(&rngx);
		i = 0;
		while ((cp = next_cell_in_range()))
			i++;
		if (num < i)
			num = i;
	}

	ys = (double *)calloc(NUM_DATASETS * num, sizeof(double));
#define	YS(a,b)	ys[NUM_DATASETS * (a-1) + b]

	for (r = 1; r < NUM_DATASETS; r++) {
		rngx = graph_get_data(r);

		if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
			continue;

		make_cells_in_range(&rngx);
		i = 0;
		while ((cp = next_cell_in_range())) {
			YS(r,i) = float_cell(cp);
			i++;
		}
	}

#if 0
	fprintf(stderr, "PuBarChart: num %d, nsets %d\n", num, nsets);
	for (r = 1; r < NUM_DATASETS; r++) {
		fprintf(stderr, "Data Set %d :\t", r);
		if (! dsvalid[r]) {
			fprintf(stderr, "invalid\n");
			continue;
		}
		for (i=0; i<num; i++)
			fprintf(stderr, "%f\t", YS(r,i));
		fprintf(stderr, "\n");
	}
#endif

	/* Find Y boundaries */
	if (stacked) {
		ymin = ymax = 0.0;
		for (i=0; i<num; i++) {
			y = 0.0;
			for (r = 1; r < NUM_DATASETS; r++)
				if (dsvalid[r])
					y += YS(r,i);
			if (ymax < y) ymax = y;
			if (ymin > y) ymin = y;
		}
	} else {
		/* FIX ME */
		for (r = 1; r < NUM_DATASETS; r++) {
			rngx = graph_get_data(r);

		    if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
			continue;

		/* Find Y boundaries */
			make_cells_in_range(&rngx);
			num = 0;
			while ((cp = next_cell_in_range())) {
				y = float_cell(cp);
				if (num == 0) y1 = y2 = y;
				if (y < y1) y1 = y;
				if (y > y2) y2 = y;
				num++;
			}
		}
	}

	pl_fspace(-1., -1., 11., 11.);			/* FIX ME */
	pl_pencolorname(defaultcolor);
	pl_fline(0., 0., 0., 10.);
	pl_fline(0., 0., 10., 0.);

	for (i=0; i<num; i++) {
		for (r = 1; r < NUM_DATASETS; r++) {
			pl_fillcolorname(colors[r % ncolors]);
#define	TO_X(ii)	(1.5 + 10.0 * ((double)ii) / ((double)num))
			x = TO_X(i);
			if (ymax) {
				if (r > 1) {
					int ii;
					double	s = 0.0;

					for (ii=1; ii<r; ii++)
						s += YS(ii,i);
						
					pl_fbox(TO_X(i - 0.6), s / ymax * 10., TO_X(i), (s + YS(r,i)) / ymax * 10.);
				} else
					pl_fbox(TO_X(i - 0.6), 0.0, TO_X(i), YS(r,i) / ymax * 10.);
			}
		}
	}

	/* General Title */
	pl_fmove(5.0, -0.75);
	pl_alabel(1, 1, graph_get_title());

	/* X Axis Labels */
	rngx = graph_get_data(0);
	make_cells_in_range(&rngx);
	i = 1;
	while ((cp = next_cell_in_range())) {
		x = TO_X(i - 0.6);
		if (GET_TYP(cp) == TYP_STR)
			if (cp->cell_str) {
				pl_fmove(x, -0.3);
				pl_alabel(1, 1, cp->cell_str);
			}
		else
			/* ??? */ ;

		i++;
	}
	

	/* Data titles */
	pl_fmove(10.0, -0.75);
	pl_alabel(1, 1, graph_get_axis_title('x'));
	pl_fmove(0.0, 10.5);
	pl_alabel(1, 1, graph_get_axis_title('y'));

	PuClose();
	free(ys);
}

/*
 * XY charts
 */
void
PuXYChart(char *plotter, FILE *outfile)
{
	int		i, r, num;
	double		x, y, xmin, xmax, delta, *xes, ymax, ymin, oldx, oldy;
	struct rng	rngx;
	CELL		*cp;

	PuOpen(plotter, outfile);
	pl_fspace(-1., -1., 11., 11.);			/* FIX ME */

	pl_pencolorname(defaultcolor);

	pl_fline(0., 0., 0., 10.);
	pl_fline(0., 0., 10., 0.);

	/* Figure out X axis borders */
	rngx = graph_get_data(0);
	make_cells_in_range(&rngx);
	cp = next_cell_in_range();
	xmin = xmax = float_cell(cp);
	num = 0;
	while ((cp = next_cell_in_range())) {
		    y = float_cell(cp);
		    num++;
		    if (y < xmin)
			xmin = y;
		    else if (y > xmax)
			xmax = y;
	}

	xes = (double *)calloc(num, sizeof(double));
	make_cells_in_range(&rngx);
	i = 0;
	while ((cp = next_cell_in_range())) {
		    xes[i++] = float_cell(cp);
	}

{	extern int XYxAuto;
	if (XYxAuto) {
		delta = xmax - xmin;
		xmin -= delta * 0.1;
		xmax += delta * 0.1;
	} else {
		extern double XYxMin, XYxMax;

		xmin = XYxMin;	/* FIX ME */
		xmax = XYxMax;
	}
}

#if 0
	fprintf(stderr, "X range should be %f to %f\n", xmin, xmax);
#endif

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

{	extern int XYyAuto;
	if (! XYyAuto) {
		extern double XYyMin, XYyMax;

		ymin = XYyMin;	/* FIX ME */
		ymax = XYyMax;
	}
}

	    pl_pencolorname(colors[r % ncolors]);

	    make_cells_in_range(&rngx);
	    i = 0;
	    while ((cp = next_cell_in_range())) {
		    x = xes[i];
		    y = float_cell(cp);
#if 1
		    if (i > 0) {
			pl_fline(10.0 * (oldx - xmin) / (xmax - xmin),
				10.0 * (oldy - ymin) / (ymax - ymin),
				10.0 * (x - xmin) / (xmax - xmin),
				10.0 * (y - ymin) / (ymax - ymin));
			pl_fmove(
				10.0 * (x - xmin) / (xmax - xmin),
				10.0 * (y - ymin) / (ymax - ymin));
		    }
#else
			pl_fmarker(10.0 * (x - xmin) / (xmax - xmin),
				10.0 * (y - ymin) / (ymax - ymin),
				2 /* + marker */,
				0.2);
#endif
#if 0
			{ char txt[30];
			sprintf(txt, " (%1.1f,%1.1f)", x, y);
			pl_alabel('l', 'c', txt);
			}
#endif

		    oldx = x;
		    oldy = y;

		    i++;
	    }
#if 1
	    pl_endpath();
#endif
	}

	/* Title */
	pl_fmove(5.0, -0.5);
	pl_alabel(1, 1, graph_get_title());

	/* Data titles */
	pl_fmove(10.0, -0.5);
	pl_alabel(1, 1, graph_get_axis_title('x'));
	pl_fmove(0.0, 10.5);
	pl_alabel(1, 1, graph_get_axis_title('y'));

	PuClose();
	free((void *)xes);
}
#endif	/* HAVE_LIBPLOT */
