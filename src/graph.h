#ifndef GRAPHH
#define GRAPHH

/*
	$Id: graph.h,v 1.6 2000/03/03 07:52:40 danny Exp $

	Copyright (C) 1993-1998 Free Software Foundation, Inc.

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
#include "cell.h"

extern char * graph_axis_name [graph_num_axis];
extern char * graph_order_name [graph_num_orders];


extern int graph_ornt_row_magic [graph_num_pair_orientations];
extern int graph_ornt_col_magic [graph_num_pair_orientations];

extern char *pair_order_name [graph_num_pair_orders];


typedef void (*fpi_thunk) (void * rock, CELL * cp, CELLREF, CELLREF);


extern enum graph_axis chr_to_axis (int c);
extern enum graph_ordering chr_to_graph_ordering (int c);
extern enum graph_pair_ordering chrs_to_graph_pair_ordering (int pair, int dir);
extern char * graph_quoted_str (char *str);
extern void graph_x11_mono (void);
extern void graph_x11_color (void);
extern void graph_postscript (char * file, int kind, int spectrum, char * font, int pt_size);
extern void graph_set_axis_title (int axis_c, char * title);
extern void graph_set_logness (int axis_c, int explicit, int newval);
extern void graph_check_range (char * val);
extern void graph_set_axis_lo (int axis_c, char * val);
extern void graph_set_axis_hi (int axis_c, char * val);
extern void graph_set_axis_symbolic (int axis_c, struct rng * rng, int ordering_c) ;
extern void graph_set_axis_labels (int axis_c, struct rng * rng, int pair, int dir);
extern void graph_default_axis_labels (int axis_c);
extern int graph_check_style (char * name);
extern void graph_set_style (int data_set, char * style);
extern void graph_set_data_title (int data_set, char * title);
extern void graph_set_data (int data_set, struct rng * rng, int pair, int dir);
extern void graph_presets (void);
extern void graph_clear_datasets (void);
extern void init_graphing (void);
extern void graph_make_info (void);
extern void for_pairs_in (struct rng * rng, enum graph_pair_ordering order, fpi_thunk thunk, void * frame);
extern void graph_plot (void);

extern char *graph_get_axis_title(int axis_c);
extern void graph_set_title(char *);
extern char *graph_get_title(void);
extern int graph_get_logness(int axis_c);
extern char *graph_get_data_title (int data_set);
extern struct rng graph_get_data(int data_set);

void graph_set_axis_auto(int axis, int set);
int graph_get_axis_auto(int axis);

double graph_get_axis_lo(int axis);
double graph_get_axis_hi(int axis);
void graph_set_linetooffscreen(int set);
int graph_get_linetooffscreen(void);

typedef void (*plotter) (void);

typedef struct oleo_graph {
	plotter	plot_fn;
	struct line graph_axis_title [graph_num_axis];	/* `set label %s' for each axis.  */
	int graph_logness [graph_num_axis];		/* set logarithmic */
/* Arguments to `set range [%s:%s]' */
	struct line graph_rng_lo [graph_num_axis];
	struct line graph_rng_hi [graph_num_axis];
/* The ranges (if any) of the symbolic names
 * for integer coordinates starting at 0.
 * If none, these will have lr == NON_ROW.
 */
	struct rng graph_axis_symbols [graph_num_axis];
	enum graph_ordering graph_axis_ordering [graph_num_axis];
/* Names to print along the axes */
	struct rng graph_axis_labels [graph_num_axis];
	enum graph_pair_ordering graph_axis_label_order [graph_num_axis];
/* plot .... with %s */
	struct line graph_style [NUM_DATASETS];
	struct line graph_title [NUM_DATASETS];
	struct rng graph_data [NUM_DATASETS];
	enum graph_pair_ordering graph_data_order [graph_num_axis];

/* Numeric values for axis ranges */
	double	graph_axis_lo[graph_num_axis],
		graph_axis_hi[graph_num_axis];
} oleo_graph;
#endif  /* GRAPHH */
