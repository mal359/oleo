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

#if 0
static float default_pswid = 8.5 * 72.;
static float default_pshgt = 11. * 72.;
#else
extern float default_pswid;
extern float default_pshgt;
#endif

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
#if 1
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
print_region_cmd (struct rng *rng, FILE *fp)
{
  psprint_region (fp, rng, default_pswid, default_pshgt, 0);
}
