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

#include "global.h"
#include "format.h"
#include "line.h"
#include "cell.h"
#include "cmd.h"
#include "io-term.h"
#include "io-abstract.h"
#include "io-generic.h"

char *
fmt_to_str (int fmt)
{
  char *ptr;
  static char buf[30];
  char nbuf[10];
  int format;

  format = (fmt & FMT_MASK) >> FMT_SHIFT;

  switch (format)
    {
    case FMT_DEF:		/* Default */
      ptr = "default";
      break;
    case FMT_HID:		/* Hidden */
      ptr = "hidden";
      break;
    case FMT_GPH:		/* Graph */
      ptr = "graph";
      break;
    case FMT_INT:		/* Integer */
      ptr = "integer";
      break;
    case FMT_DEC:		/* Decimal */
      ptr = "decimal";
      break;
    case FMT_FLT:		/* Floating-point */
      ptr = "float";
      sprintf(nbuf, "%d", FLOAT_PREC);
      break;
    case FMT_USR:		/* User */
      ptr = "user-";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    case FMT_DOL:		/* Dollar */
      ptr = "dollar.";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    case FMT_CMA:		/* Comma */
      ptr = "comma.";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    case FMT_PCT:		/* Percent */
      ptr = "percent.";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    case FMT_FXT:		/* Fixed */
      ptr = "fixed.";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    case FMT_EXP:		/* Exponential */
      ptr = "exponent.";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    case FMT_GEN:		/* General */
      ptr = "general.";
      sprintf (nbuf, "%d", (fmt & PREC_MASK));
      break;
    default:
      io_error_msg ("Unknown format %d (%x)", fmt, fmt);
      ptr = "UNKNOWN";
      break;
    }
  sprintf (buf, "%s%s", ptr, nbuf);
  return buf;
}

struct fmt
{
  int fmt;
  char **strs;
};

static char *def_names[] =	{"default", "def", "D", 0};
static char *hid_names[] =	{"hidden", "hid", "H", 0};
static char *gph_names[] =	{"graph", "gph", "*", 0};
static char *int_names[] =	{"integer", "int", "I", 0};
static char *dec_names[] =	{"decimal", "dec", 0};

static struct fmt simple[] =
{
  {FMT_DEF, def_names},
  {FMT_HID, hid_names},
  {FMT_GPH, gph_names},
  {FMT_INT, int_names},
  {FMT_DEC, dec_names},
  {0, 0}
};

char *gen_names[] =	{"general.",	"gen.", "G", 0};
char *dol_names[] =	{"dollar.",	"dol.", "$", 0};
char *cma_names[] =	{"comma.",	"com.", ",", 0};
char *pct_names[] =	{"percent.",	"pct.", "%", 0};
char *fxt_names[] =	{"fixed.",	"fxt.", "F", 0};
char *exp_names[] =	{"exponent.",	"exp.", "E", 0};

static struct fmt withprec[] =
{
  {FMT_DOL , dol_names},
  {FMT_CMA , cma_names},
  {FMT_PCT , pct_names},
  {FMT_FXT , fxt_names},
  {FMT_EXP , exp_names},
  {FMT_GEN , gen_names},
  {0, 0}
};

int
str_to_fmt (char *ptr)
{
  struct fmt *f;
  char **strs;
  int n;
  int ret;
  char *p1, *p2;

  for (f = simple; f->strs; f++)
    {
      for (strs = f->strs; *strs; strs++)
	{
	  if (*ptr != **strs)
	    continue;
	  for (p1 = ptr, p2 = *strs; *p1 == *p2 && *p1; p1++, p2++)
	    ;
	  if (*p1 == '\0' && *p2 == '\0')
	    return ((f->fmt) << FMT_SHIFT); 
	}
    }
  if (!strncmp (ptr, "user-", 5))
    {
      ptr += 5;
      n = astol (&ptr);
      if (*ptr || n < 1 || n > 16)
	return -1;
      return (FMT_USR << FMT_SHIFT) + n;
    }
  for (f = withprec, ret = 0; !ret && f->strs; f++)
    {
      for (strs = f->strs; *strs; strs++)
	{
	  if (*ptr != **strs)
	    continue;
	  for (p1 = ptr, p2 = *strs; *p2 && *p1 == *p2; p1++, p2++)
	    ;
	  if (!*p2)
	    {
	      ret = f->fmt;
	      ptr = p1;
	      break;
	    }
	}
    }

  if (!ret || !*ptr)
    return -1;
  if (!strcmp (ptr, "float") || !strcmp (ptr, "f"))
    {
      n = FLOAT_PREC;
    }
  else
    {
      n = astol (&ptr);
      if (*ptr || n < 0 || n > 14)
	return -1;
    }
  return (ret << FMT_SHIFT) + n;
}

char *
jst_to_str (int jst)
{
  if (jst == JST_DEF)
    return "default";
  if (jst == JST_LFT)
    return "left";
  if (jst == JST_RGT)
    return "right";
  if (jst == JST_CNT)
    return "center";
  return "unknown";
}

int
chr_to_jst (int chr)
{
  if (chr == 'd' || chr == 'D')
    return JST_DEF;
  if (chr == 'l' || chr == 'L')
    return JST_LFT;
  if (chr == 'r' || chr == 'R')
    return JST_RGT;
  if (chr == 'c' || chr == 'C')
    return JST_CNT;
  return -1;
}
