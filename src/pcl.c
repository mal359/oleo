/*
 *  $Id: pcl.c,v 1.5 1999/05/12 19:48:26 danny Exp $
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

static char rcsid[] = "$Id: pcl.c,v 1.5 1999/05/12 19:48:26 danny Exp $";

#include <stdio.h>

#include "config.h"
#include "global.h"

#include "print.h"

void PCLJobHeader(char *str, int npages, FILE *fp)
{
	/* Printer reset */
	fputc('\033', fp);
	fputc('E', fp);
}

void PCLJobTrailer(int npages, FILE *fp)
{
}

void PCLPageHeader(char *str, FILE *fp)
{
}

void PCLPageFooter(char *str, FILE *fp)
{
	/* Form feed */
	fputc('\f', fp);
}

void PCLField(char *str, int wid, int justify, int rightborder, FILE *fp)
{
	char	format[16];
	sprintf(format, "%%%ds", wid);
	fprintf(fp, format, str);
}

void PCLBorders(FILE *fp)
{
}

void PCLFont(char *family, char *slant, int size, FILE *fp)
{
	/* Iso Latin 1 */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('0', fp);
	fputc('N', fp);

	/* Typeface */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('s', fp);
	fputc('3', fp);	/* Courier */
	fputc('T', fp);

	/* Primary size */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('s', fp);
	fprintf(fp, "%d", size);
	fputc('V', fp);
}

void PCLNewLine(int ht, FILE *fp)
{
}

int PCLPrinterJustifies(void)
{
	return 1;
}

void PCLPaperSize(int wid, int ht, FILE *fp)
{
	/* A4 */
	fputc('\033', fp);
	fputc('&', fp);
	fputc('l', fp);
	fputc('2', fp);
	fputc('6', fp);
	fputc('A', fp);
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
	&PCLPrinterJustifies,
	&PCLPaperSize
};
