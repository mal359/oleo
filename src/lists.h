#ifndef LISTSH
#define LISTSH

/*	Copyright (C) 1992, 1993 Free Software Foundation, Inc.

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
/*  t. lord	Sun Aug  9 20:38:55 1992	*/


extern float height_scale;
extern float width_scale;
extern float user_height_scale;
extern float user_width_scale;
extern int display_formula_mode;

extern void flush_everything (void);
extern void find_cells_in_range (struct rng *r);
extern void make_cells_in_range (struct rng *r);
extern struct cell * next_cell_in_range (void);
extern struct cell * next_row_col_in_range (CELLREF *rowp, CELLREF *colp);
extern void no_more_cells (void);
extern CELLREF max_row (CELLREF col);
extern CELLREF max_col (CELLREF row);
extern CELLREF highest_row (void);
extern CELLREF highest_col (void);
extern int get_width (CELLREF col);
extern int get_nodef_width (CELLREF col);
extern void set_width (CELLREF col, int wid);
extern void find_widths (CELLREF lo, CELLREF hi);
extern int next_width (CELLREF *posp);
extern void shift_widths (int over, CELLREF lo, CELLREF hi);
extern int get_height (CELLREF row);
extern int get_nodef_height (CELLREF row);
extern void set_height (CELLREF row, int hgt);
extern void set_user_scales (double hs, double ws);
extern int get_scaled_height (CELLREF r);
extern int get_scaled_width (CELLREF c);
extern void find_heights (CELLREF lo, CELLREF hi);
extern int next_height (CELLREF *posp);
extern void shift_heights (int dn, CELLREF lo, CELLREF hi);

#endif
