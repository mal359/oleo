#undef	OLD_PRINT

/*
 * Copyright (C) 1992, 1993, 1999 Free Software Foundation, Inc.
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
		wid, ht, npages;

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
	CurrentPrintDriver->font("Courier-10", fp);

	CurrentPrintDriver->page_header("page 1", fp);

	for (rr = print->lr; rr <= print->hr; rr++) {
		ht = get_height(rr) * 20;

		for (cc = print->lc; cc <= print->hc; cc++) {
			wid = get_width(cc);
			cp = find_cell (rr, cc);
			ptr = print_cell (cp);
			lenstr = strlen (ptr);
			if (lenstr > wid)
				if (wid > 1) ptr[wid-1] = 0;
			CurrentPrintDriver->field(ptr, wid, 1, fp);
		}
		CurrentPrintDriver->newline(ht, fp);
	}
	CurrentPrintDriver->page_footer("end page 1", fp);
	CurrentPrintDriver->job_trailer(fp);
#endif
}
