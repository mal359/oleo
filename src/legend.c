/*
 * $Header: /home/matt/cvs/oleo/oleo/src/legend.c,v 1.1 2000/02/22 23:34:14 danny Exp $
 *
 * This file is part of libsciplot, one of the libraries in the GNU PlotUtils package.
 *
 * Copyright © 2000 by Danny Backx
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifdef	HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef	HAVE_LIBSCIPLOT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sciplot.h>
#include <sciplotI.h>

void sp_legend_draw(Multigrapher *mg)
{
	double	y;
	int	i;

        switch (mg->plot_type) {
        case SP_PLOT_XY:
        case SP_PLOT_PIE:
                break;
        case SP_PLOT_BAR:
        default:
                return;	/* No legend */
        }

	pl_pencolorname_r(mg->plotter, "black");	/* FIX ME */

	/* Draw rectangle */
	pl_fbox_r(mg->plotter, PLOT_SIZE * 0.8, PLOT_SIZE * 0.8, PLOT_SIZE, PLOT_SIZE);

#if 0
	fprintf(stderr, "sp_legend_draw(%d datasets, %d legend texts)\n",
		mg->datasetnum, mg->nlegend);
#endif

	for (y = 0.975 * PLOT_SIZE, i=1;
			(i <= mg->datasetnum) && (i <= mg->nlegend);
			i++, y -= 0.025 * PLOT_SIZE) {
		if (mg->legend[i]) {
			int	ly = y + 0.0125 * PLOT_SIZE;

			pl_colorname_r(mg->plotter, colorstyle[(i-1) % NO_OF_LINEMODES]);
			pl_fline_r(mg->plotter,
					0.81 * PLOT_SIZE, ly,
					0.85 * PLOT_SIZE, ly);
			pl_fmove_r(mg->plotter, 0.87 * PLOT_SIZE, y);
			pl_alabel_r(mg->plotter, 'l', 'x', mg->legend[i]);

#if 0
			fprintf(stderr, "Legend no %d, '%s', at %f %f\n",
				i, mg->legend[i],
				0.8 * PLOT_SIZE, y);
#endif
		} else {
#if 0
			fprintf(stderr, "Legend no %d - no text\n", i);
#endif
		}
	}
}

void sp_legend_label(Multigrapher *mg, int i, char *s)
{
	int	j;

//	fprintf(stderr, "sp_legend_label(%d,%s)\n", i, s);

	if (i < 0)
		return;

	if (mg->nlegend < i) {
		mg->legend = realloc(mg->legend, sizeof(char *) * (i + 1));
		for (j=mg->nlegend+1; j < i; j++) {
			mg->legend[j] = NULL;
		}
		mg->nlegend = i;
	}

	mg->legend[i] = strdup(s);
}

#endif	/* HAVE_LIBSCIPLOT */
