/*	Copyright (C) 1998-1999 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

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
 * Internal functions - initialize and close GNU PlotUtils
 */
static void
PuOpen(const char *plotter, FILE *outfile)
{
	int	r;

	handle = newpl(plotter, NULL, outfile, stderr);
	selectpl(handle);
	r = openpl();

	filltype(1);
	joinmod("round");
	flinewidth(2);
	pencolorname(defaultcolor);
}

static void
PuClose()
{
	selectpl(0);
	deletepl(handle);
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
	fspace(-10., -10., 10., 10.);			/* FIX ME */

	/* Compute total */
	num = 0;
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range())) {
fprintf(stderr, "PuPieChart #%d Data %f\n", num+1, float_cell(cp));
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

		fmove(0.0,0.0);
		fillcolorname(colors[c]);
		fcont(XY(r,curr));
		if (incr > 179) {
			farc(0.0,0.0,XY(r,curr),XY(r,curr+179.));
			curr += 179.;
			incr -= 179;
		}
		farc(0.0,0.0,XY(r,curr),XY(r,incr+curr));
		fcont(0.0,0.0);
		endpath();

		la = curr + incr / 2.0;
		lx[i] = X(8.0, RAD(la));
		ly[i] = Y(8.0, RAD(la));

		curr += incr;
		c = (c+1) % ncolors;
		i++;
	}

	/* Title */
	s = graph_get_title();
	fmove(0.,-9.);					/* FIX ME */
	alabel(1, 1, s);

	/* Place labels */
	i=0;
	rngx = graph_get_data(1);
	make_cells_in_range(&rngx);
	while ((cp = next_cell_in_range())) {
		fmove(lx[i], ly[i]);
		if (GET_TYP (cp) == TYP_STR) {
		    alabel(1, 1, cp->cell_str);
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
 *
 */
void
PuBarChart(char *plotter, FILE *outfile, int stacked)
{
	int		i;
	double		x, y, y1, y2;
	struct rng	rngx;
	CELL		*cp;

	PuOpen(plotter, outfile);
	fspace(-1., -1., 11., 11.);			/* FIX ME */

	rngx = graph_get_data(0);

	pencolorname(defaultcolor);
	fillcolorname(colors[0]);

	fline(0., 0., 0., 10.);
	fline(0., 0., 10., 0.);

	/* Find Y boundaries */
	make_cells_in_range(&rngx);
	i = 1;
	while ((cp = next_cell_in_range())) {
		y = float_cell(cp);
		if (i == 1) y1 = y2 = y;
		if (y < y1) y1 = y;
		if (y > y2) y2 = y;
		i++;
	}

	make_cells_in_range(&rngx);
	i = 1;
	while ((cp = next_cell_in_range())) {
		x = i;
		y = float_cell(cp);
		if (y2) fbox(x - 0.6, 0.0, x, y / y2 * 10.);
		i++;
	}

	/* Title */
	fmove(5.0, -0.5);
	alabel(1, 1, graph_get_title());

	/* Data titles */
	fmove(10.0, -0.5);
	alabel(1, 1, graph_get_axis_title('x'));
	fmove(0.0, 10.5);
	alabel(1, 1, graph_get_axis_title('y'));

	PuClose();
}

/*
 * XY charts
 */
void
PuXYChart(char *plotter, FILE *outfile)
{
	int		i, r;
	double		x, y, xmin, xmax, delta;
	struct rng	rngx;
	CELL		*cp;

	PuOpen(plotter, outfile);
	fspace(-1., -1., 11., 11.);			/* FIX ME */

	pencolorname(defaultcolor);

	fline(0., 0., 0., 10.);
	fline(0., 0., 10., 0.);

	/* Figure out X axis borders */
	rngx = graph_get_data(0);
	make_cells_in_range(&rngx);
	cp = next_cell_in_range();
	xmin = xmax = float_cell(cp);
	while ((cp = next_cell_in_range())) {
		    y = float_cell(cp);
		    if (y < xmin)
			xmin = y;
		    else if (y > xmax)
			xmax = y;
	}

	delta = xmax - xmin;
	xmin -= delta * 0.1;
	xmax += delta * 0.1;
	fprintf(stderr, "X range should be %f to %f\n", xmin, xmax);

	for (r = 1; r < NUM_DATASETS; r++) {
	    rngx = graph_get_data(r);

	    if (rngx.lr == 0 && rngx.lc == 0 && rngx.hr == 0 && rngx.hc == 0)
		continue;

	    pencolorname(colors[r % ncolors]);

	    make_cells_in_range(&rngx);
	    i = 1;
	    while ((cp = next_cell_in_range())) {
		    x = i;
		    y = float_cell(cp);
		    fcont(x, y);
		    i++;
	    }
	    endpath();
	}

	/* Title */
	fmove(5.0, -0.5);
	alabel(1, 1, graph_get_title());

	/* Data titles */
	fmove(10.0, -0.5);
	alabel(1, 1, graph_get_axis_title('x'));
	fmove(0.0, 10.5);
	alabel(1, 1, graph_get_axis_title('y'));

	PuClose();
}
#endif	/* HAVE_LIBPLOT */
