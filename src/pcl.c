/*
 *  $Id: pcl.c,v 1.3 1999/04/29 22:30:12 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright (C) 1999 by the Free Software Foundation, Inc.
 *  Written by Danny Backx <danny@gnu.org>.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

static char rcsid[] = "$Id: pcl.c,v 1.3 1999/04/29 22:30:12 danny Exp $";

#include <stdio.h>

#include "config.h"
#include "global.h"

#include "print.h"

void PCLJobHeader(char *str, int npages, FILE *fp)
{
}

void PCLJobTrailer(FILE *fp)
{
}

void PCLPageHeader(char *str, FILE *fp)
{
}

void PCLPageFooter(char *str, FILE *fp)
{
}

void PCLField(char *str, int wid, int justify, int rightborder, FILE *fp)
{
}

void PCLBorders(FILE *fp)
{
}

void PCLFont(char *fn, FILE *fp)
{
}

void PCLNewLine(int ht, FILE *fp)
{
}

int PCLPrinterJustifies(void)
{
	return 0;
}

struct PrintDriver PCLPrintDriver = {
	"PCL",
	&PCLJobHeader,
	&PCLJobTrailer,
	&PCLPageHeader,
	&PCLPageFooter,
	&PCLField,
	&PCLBorders,
	&PCLFont,
	&PCLNewLine,
	&PCLPrinterJustifies
};
