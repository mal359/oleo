#undef	OLD_PRINT

/*
 * Copyright (C) 1992, 1993, 1999 Free Software Foundation, Inc.
 *
 * $Id: print.c,v 1.8 1999/05/06 22:18:14 danny Exp $
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

#include <stdio.h>
#include <ctype.h>
#include "display.h"
#include "font.h"
#include "global.h"
#include "cmd.h"
#include "io-generic.h"
#include "io-abstract.h"
#include "io-utils.h"

#include "print.h"

/*
 * Add an include line for new printer types here
 */
#include "pcl.h"
#include "postscript.h"
#include "epson.h"
#include "prtext.h"

/*
 * Add a pointer to the printer type structure here
 */

static struct PrintDriver *Drivers[] = {
	&PostScriptPrintDriver,
	&PCLPrintDriver,
	&EpsonEscP2PrintDriver,
	&TextPrintDriver,
	NULL
};

/* That should be all you need to do for adding printer types.  */

static int NumPrintDrivers = (sizeof(Drivers) / sizeof(struct PrintDriver *)) - 1;
static struct PrintDriver *CurrentPrintDriver = &PostScriptPrintDriver;


static struct page_size size_table[] =
{
  { "letter",       612,  792     }, /* (8.5 x 11  in.)   */
  { "landscape",    792,  612     }, /* (11 x 8.5  in.)   */
  { "tabloid",      792,  1224    }, /* (11 x 17  in.)    */
  { "ledger",       1224, 792     }, /* (17 x 11  in.)    */
  { "legal",        612,  1008    }, /* (8.5 x 14  in.)   */
  { "statement",    396,  612     }, /* (5.5 x 8.5 in.)   */
  { "executive",    540,  720     }, /* (7.5 x 10  in.)   */
  { "a3",           842,  1190    },
  { "a4",           595,  842     },
  { "latex-a4",     523,  770     }, /* A4 - 1in margins all round */
  { "a5",           420,  595     },
  { "b4",           729,  1032    },
  { "b5",           516,  729     },      
  { "folio",        612,  936     }, /* (8.5 x 13  in.)   */
  { "quarto",       610,  780     }
};

float zoom = 1.0;

struct page_size *
find_size( char * size, int len )
{
  int i;
  struct page_size *p = size_table;
  
  for (i = 0;
       i < sizeof(size_table)/sizeof(struct page_size);
       i++, p++)
    if (strincmp (size, p->name, len) == 0 )
      return p;
  return 0;
}

int
PrintGetNumPageSizes(void)
{
	return sizeof(size_table) / sizeof(struct page_size);
}

char *
PrintGetPageName(int index)
{
	if (index < 0 ||
		index > (sizeof(size_table) / sizeof(struct page_size)))
	    return NULL;
	return size_table[index].name;
}

void
PrintSetType(char *format)
{
	int	i;

	for (i=0; i<NumPrintDrivers; i++)
		if (strcmp(Drivers[i]->name, format) == 0) {
			CurrentPrintDriver = Drivers[i];
			return;
		}
}

/* Allow Motif interface to query supported printers */
char *
PrintGetType(int i)
{
	if (i < 0 || i >= NumPrintDrivers)
		return NULL;
	return Drivers[i]->name;
}

float default_pswid = 8.5 * 72.;
float default_pshgt = 11. * 72.;

void
PrintSetPageSize(float wid, float ht)
{
	default_pswid = wid;
	default_pshgt = ht;
}

void
PrintSetPage(char *page)
{
	int	i;

	for (i=0; i < (sizeof(size_table) / sizeof(struct page_size)); i++)
		if (strcmp(size_table[i].name, page) == 0) {
			default_pswid = size_table[i].wid;
			default_pshgt = size_table[i].hgt;

			return;
		}
	/* This should never happen */
      io_error_msg("PrintSetPage: Bad page size %s.", page);
}

/*
 * Front end to PostScript printing :
 *	set_page_size_cmd
 *	psprint_region_cmd
 */

void
set_page_size_cmd (char * whole_str)
{
#ifdef	OLD_PRINT
	psprint_set_page_size_cmd(whole_str);
#else
  char * str = whole_str;
  float neww;
  float newh;
  while (*str && isspace(*str))
    ++str;
  if (!isdigit (*str) && *str != '.')
    {
      char * end = str;
      struct page_size * ps;
      while (*end && !isspace(*end))
	++end;
      ps = find_size (str, end - str);
      if (ps)
	{
	  default_pswid = ps->wid;
	  default_pshgt = ps->hgt;
	  return;
	}
      io_error_msg
	("Bad page size (should look like `8.5 x 11' or `21.6 x 28c'): %s.",
	 whole_str);
      return;
    }
  neww = atof (str);
  while (*str && isdigit(*str))
    ++str;
  if (*str == '.')
    {
      ++str;
      while (isdigit (*str))
	++str;
    }
  while (*str && isspace(*str))
    ++str;
  if (*str == 'x')
    {
      ++str;
      while (*str && isspace(*str))
	++str;
    }
  if (!isdigit (*str) && *str != '.')
    {
      io_error_msg
	("Bad page size (should look like `8.5 x 11' or `21.6 x 28c'): %s.",
	 whole_str);
      return;
    }
  newh = atof (str);
  while (*str && isdigit(*str))
    ++str;
  if (*str == '.')
    {
      ++str;
      while (*str && isdigit (*str))
	++str;
    }
  while (*str && isspace(*str))
    ++str;
  if (*str == 'c')
    {
      neww *= .3937;
      newh *= .3937;
    }
  if (*str != 'p')
    {
      default_pswid = neww * 72;
      default_pshgt = newh * 72;
    }
#endif
}

void 
print_region_cmd (struct rng *print, FILE *fp)
{
	CELLREF rr, cc;
	CELL *cp;
	char *ptr;
	int w, j, lenstr;
	int spaces;
	CELLREF c_lo, c_hi;
	int	print_width, print_height, totht, totwid,
		ht, npages;
	char	pg[32];

#ifdef OLD_PRINT
	psprint_region (fp, rng, default_pswid, default_pshgt, 0);
#else

	/* Figure out page width and height */
	print_width = default_pswid;
	print_height = default_pshgt;

	/* Figure out #pages */
		/* Depends on all fonts used, but also on zoom options chosen */
		/* Sometimes preset (zoom -> 1 page),
		 * sometimes need to scan the whole range */
		/* FIX ME */
	totht = totwid = 0;
	for (rr = print->lr; rr <= print->hr; rr++) {
		totht += get_height(rr) * 20;	/* FIX ME font height */
	}
	for (cc = print->lc; cc <= print->hc; cc++) {
		totwid += get_width(cc) * 10;	/* FIX ME font cell width */
	}
	npages = ((print_height - 1 + totht) / print_height)
		* ((print_width - 1 + totwid) / print_width);

	/* Start Printing */
	CurrentPrintDriver->job_header("title (print_region_cmd)", npages, fp);

	/* Set default font */
	CurrentPrintDriver->font("Courier", NULL, 10, fp);

	/* Adapted from txt_print_region */
	npages = 1;
	for (c_lo = print->lc, c_hi = 0; c_hi != print->hc; c_lo = c_hi + 1) {
	    w = 0;

	    /* Figure out which columns we can print */
	    cc = c_lo;
	    for (w = get_width (cc); w <= print_width && cc <= print->hc;
			w += get_width(++cc) * 10)
		;
	    if (cc != c_lo)
		--cc;
	    c_hi = cc;		/* The last column to print on the current page */

	    totht = 0;		/* Start counting height taken up on a page */
	    for (rr = print->lr; rr <= print->hr; rr++) {
		if (totht == 0) {
			sprintf(pg, "page %d", npages);
			CurrentPrintDriver->page_header(pg, fp);
			npages++;
		}

		ht = get_height(rr) * 20;
		spaces = 0;
		for (cc = c_lo; cc <= c_hi; cc++) {
		    w = get_width (cc);
		    if (!w)
			continue;
		    cp = find_cell (rr, cc);
		if (CurrentPrintDriver->printer_justifies()) {
		    if (!cp || !GET_TYP (cp)) {
			CurrentPrintDriver->field("", w, 0, 1, fp);
		    } else {
			ptr = print_cell (cp);
			if (strlen(ptr) > w)
				if (w > 1) ptr[w-1] = 0;
			CurrentPrintDriver->field(ptr, w, GET_JST(cp), 1, fp);
		    }
		} else {
		    if (!cp || !GET_TYP (cp)) {
			spaces += w;
			continue;
		    }
		    ptr = print_cell (cp);
		    lenstr = strlen (ptr);
		    if (lenstr == 0) {
			spaces += w;
			continue;
		    }
		    if (spaces) {
			fprintf (fp, "%*s", spaces, "");
			spaces = 0;
		    }
		    j = GET_JST (cp);
		    if (j == JST_DEF)
			j = default_jst;
		    if (lenstr <= w - 1) {
			if (j == JST_LFT) {
			    fprintf (fp, "%s", ptr);
			    spaces = w - lenstr;
			} else if (j == JST_RGT) {
			    fprintf (fp, "%*s", w - 1, ptr);
			    spaces = 1;
			} else if (j == JST_CNT) {
			    w = (w - 1) - lenstr;
			    fprintf (fp, "%*s", w / 2 + lenstr, ptr);
			    spaces = (w + 3) / 2;
			}
		    } else {
			CELLREF ccc = cc;
			CELL *ccp;
			int tmp_wid;
			unsigned int ww;

			for (ww = w;; tmp_wid = get_width (ccc), w += tmp_wid, spaces -= tmp_wid) {
			    if (lenstr < w - 1)
				break;
			    if (++ccc > c_hi)
				break;
			    ccp = find_cell (rr, ccc);
			    if (!ccp || !GET_TYP (ccp) || GET_FORMAT (ccp) == FMT_HID)
				continue;
			    if (GET_FORMAT (ccp) == FMT_DEF && default_fmt == FMT_HID)
				continue;
				break;
			}
			if (lenstr > w - 1) {
				if (GET_TYP (cp) == TYP_FLT) {
				    ptr = adjust_prc (ptr, cp, w - 1, ww - 1, j);
				    lenstr = strlen (ptr);
				} else if (GET_TYP (cp) == TYP_INT) {
				    ptr = numb_oflo;
				    lenstr = 80;
				}
				fprintf (fp, "%.*s", w - 1, ptr);
				if (lenstr < w)
				    spaces += w - lenstr;
				else
				    spaces++;
			} else {
			    fprintf (fp, "%s", ptr);
			    spaces += w - lenstr;
			}
		    }
		}
		}
		CurrentPrintDriver->newline(ht, fp);
		totht += ht;
		if (totht >= print_height) {
			totht = 0;

			sprintf(pg, "page %d", npages);
			CurrentPrintDriver->page_footer(pg, fp);
		}
	    }	/* Rows on a page */

	    if (totht != 0) {
		totht = 0; 
		sprintf(pg, "page %d", npages);
		CurrentPrintDriver->page_footer(pg, fp);
	    }
	}

	if (totht != 0) {
	    sprintf(pg, "end page %d", npages);
	    CurrentPrintDriver->page_footer(pg, fp);
	}
	CurrentPrintDriver->job_trailer(npages-1, fp);
#endif
}
