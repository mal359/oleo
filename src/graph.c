/*	Copyright (C) 1993 Free Software Foundation, Inc.

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

/* Parameters for the next graph that will be drawn. */

/* This determines the type and destination of the ouput.
 * It should be the argument to a  `set term %s'.  For posctscript graphs,
 * we slip in a `set output %s' as well.
 */
static struct line graph_term_cmd;
static struct line graph_output_file;

typedef void (*plotter) (void);

struct get_symbols_frame;
struct write_data_frame;
static void get_symbols_thunk (struct get_symbols_frame * fr,
			       CELL * cell, CELLREF r, CELLREF c);
static void write_data_thunk (struct write_data_frame * fr,
			      CELL * y_cell, CELLREF r, CELLREF c) ;
static void spew_gnuplot (FILE  * fp, struct line * data_files, char * dir,
			  char * dbase); 
static void spew_for_x (void);
static void spew_for_ps (void);

static plotter plot_fn = 0;

char * gnuplot_program = "gnuplot";
char * gnuplot_shell = "/bin/sh";
char * rm_program = "/bin/rm";

char * graph_axis_name [graph_num_axis] =
{
  "x", "y"
};

char *
graph_order_name [graph_num_orders] =
{
  "rows",
  "columns",
};

char *graph_pair_order_name[graph_num_pair_orders] =
{
  "rows of hz pairs",
  "rows of vt pairs",
  "enumerated rows",
  "cols of hz pairs",
  "cols of vt pairs",
  "enumerated cols"
};

int graph_ornt_row_magic [graph_num_pair_orientations] = { 0, 1, 0 };
int graph_ornt_col_magic [graph_num_pair_orientations] = { 1, 0, 0 };


/* `set label %s' for each axis.
 */
static struct line graph_axis_title [graph_num_axis];

/* set logarithmic */
static int graph_logness [graph_num_axis];

/* Arguments to `set range [%s:%s]' */
static struct line graph_rng_lo [graph_num_axis];
static struct line graph_rng_hi [graph_num_axis];

/* The ranges (if any) of the symbolic names
 * for integer coordinates starting at 0.
 * If none, these will have lr == NON_ROW.
 */
static struct rng graph_axis_symbols [graph_num_axis];
static enum graph_ordering graph_axis_ordering [graph_num_axis];

/* Names to print along the axes */
static struct rng graph_axis_labels [graph_num_axis];
static enum graph_pair_ordering graph_axis_label_order [graph_num_axis];

/* Parameters for each dataset. */
#define NUM_DATASETS 10

/* plot .... with %s */
static struct line graph_style [NUM_DATASETS];
static struct line graph_title [NUM_DATASETS];
static struct rng graph_data [NUM_DATASETS];
static enum graph_pair_ordering graph_data_order [graph_num_axis];




enum graph_axis
chr_to_axis (int c)
{
  if (isupper (c))
    c = tolower (c);
  switch (c)
    {
    case 'x':
      return graph_x;
    case 'y':
      return graph_y;
    }
  io_error_msg ("Unkown graph-axis `%c' (try `x' or `y')", c);
  return graph_x;		/* not reached, actualy. */
}


enum graph_ordering
chr_to_graph_ordering (int c)
{
  if (isupper (c))
    c = tolower (c);
  switch (c)
    {
    case 'r':
      return graph_rows;
    case 'c':
      return graph_cols;
    }
  io_error_msg ("Unknown cell ordering `%c' (try `r' or `c').", c);
  return graph_rows;		/* not reached */
}

enum graph_pair_ordering
chrs_to_graph_pair_ordering (int pair, int dir)
{
  int pair_offset;
  if (isupper (pair))
    pair = tolower (pair);
  if (isupper (dir))
    dir = tolower (dir);

  switch (pair)
    {
    case 'h':
      pair_offset = graph_hz;
      break;
    case 'v':
      pair_offset = graph_vt;
      break;
    case 'i':
      pair_offset = graph_implicit;
      break;
    default:
      pair_offset = graph_implicit;
      io_error_msg
	("graph.c: invalid pair ording `%c' (wanted h, v, or i)", pair);
      /* no deposit... */
    }

  return PAIR_ORDER(chr_to_graph_ordering (dir), pair_offset);
}

char *
line_to_q_char (struct line line)
{
  static struct line tmp_line;
  char *str;
  
  str = line.buf;

  set_line (&tmp_line, "\"");
  while (*str)
    {
      switch (*str)
	{
	case '"':
/* gnupolot doesn't like escaped quotation marks in titles anyway.
 * If you want to support them, you'll have to protect against
 * progressive buildup, since this stores the post-escaping string
 * back into the original char.
 */
/*        catn_line (&tmp_line, "\\\"", 2); */
	  break;
	default:
	  catn_line (&tmp_line, str, 1);
	  break;
	}
      ++str;
    }
  catn_line (&tmp_line, "\"", 1);

  set_line(&line, tmp_line.buf);

  return line.buf;
}

char *
char_to_q_char (char *str)
{
  char *tmp;

  static struct line tmp_line;
  set_line (&tmp_line, str);
  tmp = line_to_q_char (tmp_line);
  return (tmp);
}


void
graph_x11_mono (void)
{
  set_line (&graph_term_cmd, "x11 # b/w");
  set_line (&graph_output_file, "");
  plot_fn = spew_for_x;
}


void
graph_x11_color (void)
{
  set_line (&graph_term_cmd, "X11 # color");
  set_line (&graph_output_file, "");
  plot_fn = spew_for_x;
}


void
graph_postscript (char * file, int kind, int spectrum, char * font, int pt_size)
{
  if (isupper (kind))
    kind = tolower (kind);
  if (isupper (spectrum))
    spectrum = tolower (spectrum);
  if (!index ("led", kind))
    io_error_msg
      ("Incorrect postscript graph type %c (should be one of l, e, or c)",
       kind);
  if (!index ("mc", spectrum))
    io_error_msg
      ("Incorrect postscript color type %c (should be either m or c)",
       spectrum);
  sprint_line (&graph_term_cmd,
	       "postscript %c %c %s %d  # Postscript",
	       kind, spectrum, char_to_q_char (font), pt_size);
  set_line (&graph_output_file, file);
  plot_fn = spew_for_ps;
}



void
graph_set_axis_title (int axis_c, char * title)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  set_line (&graph_axis_title [axis], char_to_q_char (title));
}

void
graph_set_logness (int axis_c, int explicit, int newval)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  static struct line msg_buf;

  if (!explicit)
    newval = !graph_logness [axis];
  else
    newval = (newval > 0);

  graph_logness [axis] = newval;
  sprint_line (&msg_buf, "%slogarithmic %s%s",
	       ((graph_logness [graph_x] || graph_logness [graph_y])
		? "" : "no"),
	       graph_logness [graph_x] ? "x" : "",
	       graph_logness [graph_y] ? "y" : "");
  io_info_msg ("set %s", msg_buf.buf);
}

void
graph_check_range (char * val)
{
  if (says_default (val))
    return;
  while (isspace (*val)) ++val;
  if (*val == '-') ++val;
  while (isspace (*val)) ++val;
  if (!isdigit (*val))
    io_error_msg
      ("Illegal range specifier %s (should be a numer or `def').", val);
  else
    {
      while (*val)
	if (isspace (*val) || isdigit (*val))
	  ++val;
	else
	  break;
      while (isspace (*val)) ++val;
      if (*val == '.') ++val;
      while (*val)
	if (isspace (*val) || isdigit (*val))
	  ++val;
	else
	  io_error_msg
	    ("Illegal range specifier %s (should be a numer or `def').", val);
    }
	    
}


void
graph_set_axis_lo (int axis_c, char * val)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  graph_check_range (val);
  set_line (&graph_rng_lo [axis], val);
  graph_axis_symbols [axis].lr = NON_ROW;
}


void
graph_set_axis_hi (int axis_c, char * val)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  graph_check_range (val);
  set_line (&graph_rng_hi [axis], val);
  graph_axis_symbols [axis].lr = NON_ROW;
}

void
graph_set_axis_symbolic (int axis_c, struct rng * rng, int ordering_c) 
{
  enum graph_axis axis = chr_to_axis (axis_c);
  enum graph_ordering ordering = chr_to_graph_ordering (ordering_c);
  int top = (rng->hr - rng->lr + 1) * (rng->hc - rng->lc + 1) - 1;
  char buf [64];

  sprintf (buf, "%d.5", top);
  graph_set_axis_lo (axis, "-0.5");
  graph_set_axis_hi (axis, buf);
  graph_axis_symbols [axis] = *rng;
  graph_axis_ordering [axis] = ordering;
}

void
graph_set_axis_labels (int axis_c, struct rng * rng, int pair, int dir)
{
  enum graph_pair_ordering order = chrs_to_graph_pair_ordering (pair, dir);
  enum graph_axis axis = chr_to_axis (axis_c);
  graph_axis_labels [axis] = *rng;
  graph_axis_label_order [axis] = order;
}

void
graph_default_axis_labels (int axis_c)
{
  enum graph_axis axis = chr_to_axis (axis_c);
  graph_axis_labels [axis].lr = NON_ROW;
}



static char * graph_plot_styles [] =
{
  "lines",
  "points",
  "linespoints",
  "impulses",
  "dots",
   0
};

int
graph_check_style (char * name)
{
  int x =
    words_member (graph_plot_styles, parray_len (graph_plot_styles), name);
  if (x < 0)
    io_error_msg
      ("Invalid line style %s. (Try M-x describe-function set-graph-style).");
  return x;
}


void
graph_set_style (int data_set, char * style)
{
  int x = graph_check_style (style);
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-style -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
  set_line (&graph_style [data_set], graph_plot_styles [x]);
}

void
graph_set_data_title (int data_set, char * title)
{
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-title -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
  set_line (&graph_title [data_set], title);
}

void
graph_set_data (int data_set, struct rng * rng, int pair, int dir)
{
  enum graph_pair_ordering order = chrs_to_graph_pair_ordering (pair, dir);
  if ((data_set < 0) || (data_set >= NUM_DATASETS))
    io_error_msg
      ("set-graph-data -- data-set out of range: %d (should be in [0..%d])",
       data_set, NUM_DATASETS);
  graph_data [data_set] = *rng;
  graph_data_order [data_set] = order;
}




void
graph_presets (void)
{
  if (using_curses)
    graph_postscript ("#plot.ps", 'd', 'm', "TimesRoman", 12);
  else
    graph_x11_mono ();
  {
    enum graph_axis axis;
    for (axis = graph_x; axis < graph_num_axis; ++axis)
      {
	int axis_c = graph_axis_name [axis][0];
	graph_logness [axis] = 0;
	graph_set_axis_title (axis_c, "");
	graph_set_axis_lo (axis_c, "def");
	graph_set_axis_hi (axis_c, "def");
	graph_set_axis_title (axis_c, "");
	graph_default_axis_labels (axis_c);
      }
  }
}





void
graph_clear_datasets (void)
{
  int x;
  for (x = 0; x < NUM_DATASETS; ++x)
    {
      graph_set_style (x, "lines");
      graph_set_data_title (x, "");
      graph_data [x].lr = NON_ROW;
    }
}


void
init_graphing (void)
{
  gnuplot_program = getenv ("GNUPLOT_PROG");
  if (!gnuplot_program)
    gnuplot_program = "gnuplot";
  gnuplot_shell = getenv ("GNUPLOT_SHELL");
  if (!gnuplot_shell)
    gnuplot_shell = "/bin/sh";
  rm_program = getenv ("RM_PROG");
  if (!rm_program)
    rm_program = "/bin/rm";
  graph_presets ();
  graph_clear_datasets ();
}


void
graph_make_info (void)
{
  struct info_buffer * ib = find_or_make_info ("graphing-parameters");
  enum graph_axis axis;

  clear_info (ib);

  {
    print_info
      (ib,
       "");
    print_info
      (ib,
       "Parameter		Value");
    print_info
      (ib,
       "");

    print_info
      (ib,
       "output type		%s",
       graph_term_cmd.buf);
  }

  if (graph_output_file.buf[0])
    print_info
      (ib,
       "output file		%s",
       graph_output_file.buf);
  
  for (axis = graph_x; axis <= graph_y; ++axis)
    print_info
      (ib,
       "%s-axis title		%s",
       graph_axis_name [axis], graph_axis_title[axis].buf);

  {
    print_info
      (ib,
       "logarithmic axes	%s",
       (graph_logness [graph_x]
	? (graph_logness [graph_y] ? "x,y" : "x")
	: (graph_logness [graph_y] ? "y" : "-neither-")));

  }
  for (axis = graph_x; axis <= graph_y; ++axis)
    {
      if (graph_axis_symbols[axis].lr != NON_ROW)
	print_info
	  (ib,
	   	"%s-axis symbols	%s in %s",
	   graph_axis_name [axis],
	   graph_order_name [graph_axis_ordering [axis]],
	   range_name (&graph_axis_symbols [axis]));
      else
	print_info
	  (ib,
		"%s-axis range		[%s..%s]",
	   graph_axis_name [axis],
	   graph_rng_lo [axis].buf, graph_rng_hi [axis].buf,
	   graph_rng_hi [axis].buf, graph_rng_hi [axis].buf);
    }

  for (axis = graph_x; axis <= graph_y; ++axis)
    {
      if (graph_axis_labels[axis].lr != NON_ROW)
	print_info
	  (ib,
	   	"%s-axis labels		%s in %s",
	   graph_axis_name [axis],
	   graph_pair_order_name [graph_axis_label_order [axis]],
	   range_name (&graph_axis_labels [axis]));
    }

  {
    int x;
    for (x = 0; x < NUM_DATASETS; ++x)
      if (graph_data [x].lr != NON_ROW)
      {
	print_info (ib, "");
	print_info (ib,"Data Set %d%s%s",
		    x,
		    graph_title[x].buf[0] ? " entitled " : "",
		    graph_title[x].buf);
	print_info (ib,"  data for this set: %s in %s",
		    graph_pair_order_name [graph_data_order [x]],
		    range_name (&graph_data[x]));
	print_info (ib,"  style for this set: %s",
		    graph_style[x].buf);
	print_info (ib,"");
      }
  }
}



static FILE *
mk_tmp_file (struct line * line, char * dir, char * base)
{
  set_line (line, tmpnam (0));
  return fopen (line->buf, "w");
}


void
for_pairs_in (struct rng * rng, enum graph_pair_ordering order, fpi_thunk thunk, void * frame)
{
  CELLREF r;
  CELLREF c;
  enum graph_pair_orientation ornt = order % graph_num_pair_orientations;
  enum graph_ordering dir = ORDER_OF_PAIRS(order);
  int r_inc = 1 + graph_ornt_row_magic [ornt];
  int c_inc = 1 + graph_ornt_col_magic [ornt];
  if (dir == graph_rows)
    {
      r = rng->lr;
      while (1)
	{
	  c = rng->lc;
	  while (1)
	    {
	      CELLREF y_r = r + graph_ornt_row_magic [ornt];
	      CELLREF y_c = c + graph_ornt_col_magic [ornt];
	      CELL * cell = find_cell (y_r, y_c);
	      thunk (frame, cell, y_r, y_c);
	      if ((rng->hc - c) < c_inc)
		break;
	      c += c_inc;
	    }
	  if ((rng->hr - r) < r_inc)
	    break;
	  r += r_inc;
	}
    }
  else
    {
      c = rng->lc;
      while (1)
	{
	  r = rng->lr;
	  while (1)
	    {
	      CELLREF y_r = r + graph_ornt_row_magic [ornt];
	      CELLREF y_c = c + graph_ornt_col_magic [ornt];
	      CELL * cell = find_cell (y_r, y_c);
	      thunk (frame, cell, y_r, y_c);
	      if ((rng->hr - r) < r_inc)
		break;
	      r += r_inc;
	    }
	  if ((rng->hc - c) < c_inc)
	    break;
	  c += c_inc;
	}
    }
}

struct write_tics_frame
{
  FILE * fp;
  enum graph_axis axis;
  enum graph_pair_orientation ornt;
  int tic_cnt;
};

static void
write_tics_thunk (struct write_tics_frame * fr, CELL * cp, CELLREF r, CELLREF c)
{
  char * str = char_to_q_char (print_cell (cp));
  if (fr->tic_cnt)
    fputs (", ", fr->fp);
  fprintf (fr->fp, "%s ", str);
  if (fr->ornt == graph_implicit)
    fprintf (fr->fp, "%d", fr->tic_cnt);
  else
    {
      CELLREF x_r = r - graph_ornt_row_magic [fr->ornt];
      CELLREF x_c = c - graph_ornt_col_magic [fr->ornt];
      
      fprintf (fr->fp, "%s", print_cell (find_cell (x_r, x_c)));
    }
  ++fr->tic_cnt;
}


static void
write_tics_command (FILE * fp, enum graph_axis axis, struct rng * rng, enum graph_pair_ordering order)
{
  struct write_tics_frame fr;
  fr.fp = fp;
  fr.axis = axis;
  fr.tic_cnt = 0;
  fr.ornt = order % graph_num_pair_orientations;
  fprintf (fp, "set %stics (", graph_axis_name[axis]);
  for_pairs_in (rng, order, (fpi_thunk)write_tics_thunk, &fr);
  fputs (")\n", fp);
}

struct get_symbols_frame
{
  int symbols;
  char ** names;
};

static void
get_symbols_thunk (struct get_symbols_frame * fr,
		   CELL * cell, CELLREF r, CELLREF c)
{
  fr->names = (char **)ck_realloc (fr->names,
				   (fr->symbols + 1) * sizeof (char *));
  fr->names [fr->symbols] = ck_savestr (print_cell (cell));
  ++fr->symbols;
}


struct write_data_frame
{
  FILE * fp;
  enum graph_pair_ordering ornt;
  int data_cnt;
  struct get_symbols_frame gsf;
};

static void
write_data_thunk (struct write_data_frame * fr,
		  CELL * y_cell, CELLREF r, CELLREF c) 
{
  if (!y_cell || !GET_TYP(y_cell))
    return;
  if (fr->ornt == graph_implicit)
    fprintf (fr->fp, "%d ", fr->data_cnt);
  else
    {
      CELLREF x_r = r - graph_ornt_row_magic [fr->ornt];
      CELLREF x_c = c - graph_ornt_col_magic [fr->ornt];
      CELL * x_cell = find_cell (x_r, x_c);
      if (x_cell && GET_TYP(x_cell))
	{
	  if (graph_axis_symbols [graph_x].lr == NON_ROW)
	    fprintf (fr->fp, "%s ", print_cell (x_cell));
	  else
	    {
	      char * key = print_cell (x_cell);
	      int x;
	      for (x = 0; x < fr->gsf.symbols; ++x)
		if (stricmp (key, fr->gsf.names[x]))
		  {
		    fprintf (fr->fp, "%d ", x);
		    break;
		  }
	    }
	}
    }
  fprintf (fr->fp, " %s\n", print_cell (y_cell));
  ++fr->data_cnt;
}


static void
spew_gnuplot (FILE  * fp, struct line * data_files, char * dir, char * dbase)
{
  fprintf (fp, "set terminal %s\n", graph_term_cmd.buf);
  fprintf (fp, "set output %s\n",
	   (graph_output_file.buf[0]
	    ? line_to_q_char (graph_output_file)
	    : ""));

  {
    enum graph_axis axis;
    for (axis = graph_x; axis <= graph_y; ++axis)
      {
	fprintf (fp, "set %slabel %s\n", graph_axis_name[axis],
		 (graph_axis_title[axis].buf[0]
		  ? graph_axis_title[axis].buf
		  : ""));

	fprintf (fp, "set %srange [%s:%s]\n",
		 graph_axis_name [axis],
		 (says_default (graph_rng_lo [axis].buf)
		  ? ""
		  : graph_rng_lo[axis].buf),
		 (says_default (graph_rng_hi [axis].buf)
		  ? ""
		  : graph_rng_hi[axis].buf));
	if (   (graph_axis_symbols [axis].lr != NON_ROW)
	    && (graph_axis_labels [axis].lr == NON_ROW))
	  write_tics_command (fp, axis, &graph_axis_symbols [axis],
			      PAIR_ORDER(graph_axis_ordering[axis],
						   graph_implicit));
	else if (graph_axis_labels [axis].lr != NON_ROW)
	  write_tics_command (fp, axis,
			      &graph_axis_labels [axis],
			      graph_axis_label_order [axis]);
	else
	  fprintf (fp, "set %stics\n", graph_axis_name [axis]);
      }
    if (!(graph_logness[graph_x] && graph_logness[graph_y]))
      fprintf (fp, "set nolog %s%s\n",
	       graph_logness[graph_x] ? "" : "x",
	       graph_logness[graph_y] ? "" : "y");

    if (graph_logness[graph_x] || graph_logness[graph_y])
      fprintf (fp, "set log %s%s\n",
	       graph_logness[graph_x] ? "x" : "",
	       graph_logness[graph_y] ? "y" : "");
  }
  {
    int need_comma = 0;
    int x;
    fprintf (fp, "plot ");
    for (x = 0; x < NUM_DATASETS; ++x)
      {
	init_line (&data_files [x]);
	if (graph_data [x].lr != NON_ROW)
	  {
	    FILE * df = mk_tmp_file (&data_files [x], dir, dbase);
	    struct write_data_frame wdf;
	    if (!df)
	      {
		/* a small core leak here... */
		io_error_msg ("Error opening temp file `%s' for graph data.",
			      data_files [x].buf);
	      }
	    wdf.fp = df;
	    wdf.ornt = graph_data_order [x] % graph_num_pair_orientations;
	    wdf.data_cnt = 0;
	    wdf.gsf.symbols = 0;
	    wdf.gsf.names = 0;
	    if (graph_axis_symbols [graph_x].lr != NON_ROW)
	      for_pairs_in (&graph_axis_symbols [graph_x],
			    PAIR_ORDER (graph_axis_ordering [graph_x],
					graph_implicit),
			    (fpi_thunk)get_symbols_thunk,
			    &wdf.gsf);
	    for_pairs_in (&graph_data [x], graph_data_order [x],
			  (fpi_thunk)write_data_thunk, &wdf);
	    if (graph_axis_symbols [graph_x].lr != NON_ROW)
	      {
		int x;
		for (x = 0; x < wdf.gsf.symbols; ++x)
		  free (wdf.gsf.names[x]);
		ck_free (wdf.gsf.names);
	      }
	    fprintf (fp, "%s %s %s %s with %s",
		     need_comma ? "," : "",
		     line_to_q_char (data_files [x]),
		     graph_title[x].buf[0] ? " title " : "",
                     graph_title[x].buf[0] ? line_to_q_char (graph_title [x]) : "",
		     graph_style [x].buf);
	    need_comma = 1;
	    fclose (df);
	  }
      }
    fprintf (fp, "\n");
  }
}


static void
graph_spew_with_parameters (struct line * shell_script, struct line *
			    gp_script, char * last_cmd, char * dir, char *
			    dbase, char * gbase, char * sbase, int run_gnuplot)
{
  struct line data_files [NUM_DATASETS];
  FILE * fp;
  fp = mk_tmp_file (gp_script, dir, gbase);
  if (!fp)
    {
      /* coreleak filename */
      io_error_msg ("Error opening tmp file `%s' for plot script.",
		    gp_script->buf);
    }
  spew_gnuplot (fp, data_files, dir, dbase);
  if (last_cmd)
    fputs (last_cmd, fp);
  if (sbase)
    {
      FILE * shfp;
      init_line (shell_script);
      shfp = mk_tmp_file (shell_script, dir, sbase);
      fprintf (shfp, "#!%s\n", gnuplot_shell);
      fputs ("\n", shfp);
      if (run_gnuplot)
	fprintf (shfp, "%s %s\n", gnuplot_program, gp_script->buf);
      fprintf (shfp, "%s %s %s  ",
	       rm_program, gp_script->buf, shell_script->buf);
      {
	int x;
	for (x = 0; x < NUM_DATASETS; ++x)
	  if (data_files[x].buf)
	    fprintf (shfp, "%s ", data_files [x].buf);
      }
      fputs ("\n", shfp);
      fclose (shfp);
    }
  fclose (fp);
}


static FILE * pipe_to_gnuplot = 0;
static char * cleanup_script = 0;
static char * gnuplot_script = 0;

static void death_to_gnuplot (void);

static void
gnuplot_exception (int fd)
{
  death_to_gnuplot ();
}

static void
gnuplot_writable (int fd)
{
  FD_CLR (fd, &write_fd_set);
  file_write_hooks [fd].hook_fn = 0;
  fprintf (pipe_to_gnuplot, "\n\nload %s\n", char_to_q_char (gnuplot_script));
  fflush (pipe_to_gnuplot);
}

static void
ensure_gnuplot_pipe (void)
{
  if (!pipe_to_gnuplot)
    {
      pipe_to_gnuplot = popen (gnuplot_program, "w");
      if (!pipe_to_gnuplot)
	io_error_msg ("Can't popen gnuplot.");
    }
  if (gnuplot_script)
    {
      file_write_hooks [fileno (pipe_to_gnuplot)].hook_fn = gnuplot_writable;
      file_exception_hooks [fileno (pipe_to_gnuplot)].hook_fn = gnuplot_exception;
      FD_SET (fileno (pipe_to_gnuplot), &write_fd_set);
      FD_SET (fileno (pipe_to_gnuplot), &exception_fd_set);
    }
}

static void
death_to_gnuplot (void)
{
  if (pipe_to_gnuplot)
    {
      int fd = fileno (pipe_to_gnuplot);
      file_write_hooks [fd].hook_fn = 0;
      file_exception_hooks [fd].hook_fn = 0;
      FD_CLR (fd, &write_fd_set);
      FD_CLR (fd, &exception_fd_set);
      pclose (pipe_to_gnuplot);
    }
  pipe_to_gnuplot = 0;
  if (cleanup_script)
    system (cleanup_script);
  if (cleanup_script)
    ck_free (cleanup_script);
  if (gnuplot_script)
    ck_free (gnuplot_script);
}





static void
spew_for_x (void)
{
  struct line shell_script;
  struct line gp_script;
  init_line (&shell_script);
  init_line (&gp_script);
  graph_spew_with_parameters (&shell_script, &gp_script, "pause -1\n", 0,
			      "#data", "#plot", "#sh", 0);
  splicen_line (&shell_script, " ", 1, 0);
  splicen_line (&shell_script, gnuplot_shell, strlen (gnuplot_shell), 0);
  if (cleanup_script)
    {
      system (cleanup_script);
      free (cleanup_script);
    }
  cleanup_script = shell_script.buf;
  if (gnuplot_script)
    free (gnuplot_script);
  gnuplot_script = gp_script.buf;
  ensure_gnuplot_pipe ();
}

static void
spew_for_ps (void)
{
  struct line shell_script;
  struct line gp_script;
  init_line (&shell_script);
  init_line (&gp_script);
  graph_spew_with_parameters (&shell_script, &gp_script, 0, "", "#data",
			      "#plot", "#sh", 1); 
  splicen_line (&shell_script, " ", 1, 0);
  splicen_line (&shell_script, gnuplot_shell, strlen (gnuplot_shell), 0);
  system (shell_script.buf);
  free_line (&shell_script);
  free_line (&gp_script);
}

void
graph_plot (void)
{
  plot_fn ();
}

