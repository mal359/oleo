
/*
	$Id: oleo_plot.h,v 1.5 1999/08/31 08:44:58 danny Exp $

	Copyright (C) 1998-1999 Free Software Foundation, Inc.

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
	the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef _OLEO_PLOT_H_
#define _OLEO_PLOT_H_
/*
 * The public functions in plot.c should all have the same signature,
 * as indicated in the typedef PuFunction.
 */
typedef void (*PuFunction)(char *, FILE *);

/*
 * The first argument is the PlotUtils plotter (can be one of several character
 * strings such as "ps"; see the Plotutils documentation).
 *
 * The second argument, if relevant, is a standard file pointer which has
 * already been opened for writing.
 */
void PuPieChart(char *plotter, FILE *outfile);
void PuBarChart(char *plotter, FILE *outfile);
void PuXYChart(char *plotter, FILE *outfile);

void PlotInit(void);

struct PlotGlobalType {
	double	XYxMin, XYxMax, XYyMin, XYyMax;
	int	XYxAuto, XYyAuto, LineToOffscreen;
	int	img_width, img_height;
};

#define	XYxMin			Global->PlotGlobal->XYxMin
#define	XYxMax			Global->PlotGlobal->XYxMax
#define	XYyMin			Global->PlotGlobal->XYyMin
#define	XYyMax			Global->PlotGlobal->XYyMax
#define	XYxAuto			Global->PlotGlobal->XYxAuto
#define	XYyAuto			Global->PlotGlobal->XYyAuto
#define	LineToOffscreen		Global->PlotGlobal->LineToOffscreen

/* Defaults */
#define	PLOT_WINDOW_WIDTH	600
#define	PLOT_WINDOW_HEIGHT	600

#endif  /* _OLEO_PLOT_H_ */
