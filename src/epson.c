/*
 *  $Id: epson.c,v 1.4 1999/05/06 22:18:16 danny Exp $
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

static char rcsid[] = "$Id: epson.c,v 1.4 1999/05/06 22:18:16 danny Exp $";

#include <stdio.h>

#include "global.h"

#include "print.h"

char	*Escape = "\033";
char	*formfeed = "\f";

/* Fonts */
char	*courier = "\033k2";
char	*proportional = "\033p1";
char	*noproportional = "\033p0";
char	*Defaultfont = "\033k\002\033X\0\025\0";
char	*defaultfont = "\033k\002\033X\000\012\000";	/* play */
char	*italic = "\0334";
char	*noitalic = "\0335";

void EpsonJobHeader(char *str, int npages, FILE *fp)
{
	fprintf(fp, "%s@", Escape);
}

void EpsonJobTrailer(int npages, FILE *fp)
{
}

void EpsonPageHeader(char *str, FILE *fp)
{
}

void EpsonPageFooter(char *str, FILE *fp)
{
	fprintf(fp, "%s", formfeed);
}

void EpsonField(char *str, int wid, int justify, int rightborder, FILE *fp)
{
	char	format[16];
#if 1
	static int	play = 0;

	if (play) {
		play--;
		fprintf(fp, "%s", italic);
	} else {
		play++;
		fprintf(fp, "%s", noitalic);
	}
#endif
	sprintf(format, "%%%ds", wid);
	fprintf(fp, format, str);
}

void EpsonBorders(FILE *fp)
{
}

void EpsonFont(char *family, char *slant, int size, FILE *fp)
{
	fprintf(fp, "%s", defaultfont);
}

void EpsonNewLine(int ht, FILE *fp)
{
	fprintf(fp, "\n");
}

int EpsonPrinterJustifies(void)
{
	return 1;
}

struct PrintDriver EpsonEscP2PrintDriver = {
	"Epson ESC/P2",
	&EpsonJobHeader,
	&EpsonJobTrailer,
	&EpsonPageHeader,
	&EpsonPageFooter,
	&EpsonField,
	&EpsonBorders,
	&EpsonFont,
	&EpsonNewLine,
	&EpsonPrinterJustifies
};
