/*
 *  $Id: pcl.c,v 1.9 1999/11/04 12:51:27 danny Exp $
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

static char rcsid[] = "$Id: pcl.c,v 1.9 1999/11/04 12:51:27 danny Exp $";

#include <stdio.h>

#ifndef	TEST
#include "config.h"
#include "global.h"
#include "cell.h"
#else
#define JST_DEF		0
#define JST_LFT		1
#define JST_RGT		2
#define JST_CNT		3
#endif

#include "print.h"

#define	INITIAL_Y	10

static int	y;	/* absolute position on page */

void PCLJobHeader(char *str, int npages, FILE *fp)
{
	/* Printer reset */
	fputc('\033', fp);
	fputc('E', fp);
#if 0
	/* Landscape */
	fputc('\033', fp);
	fputc('&', fp);
	fputc('l', fp);
	fputc('1', fp);
	fputc('O', fp);
#endif

	/* Iso Latin 1 */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('0', fp);
	fputc('N', fp);
}

void PCLJobTrailer(int npages, FILE *fp)
{
}

void PCLPageHeader(char *str, FILE *fp)
{
	if (Global->need_formfeed) {
		/* Form feed */
		fputc('\f', fp);

		Global->need_formfeed = 0;
	}

	y = INITIAL_Y;
}

void PCLPageFooter(char *str, FILE *fp)
{
	/*
	 * Only set a flag here, to avoid FormFeed at the
	 * end of the document. This would just print a blank page.
	 */
	Global->need_formfeed = 1;
}

void PCLField(char *str, int wid, int rightborder,
			int xpoints, int xchars, FILE *fp)
{
	char	format[16];
	int	l;

#if 0
	fprintf(stderr, "PCLField(%s,%d,%d)\n",
		str, wid, rightborder);
#endif

	/* Absolute cursor positioning */
	fprintf(fp, "\033*p%dx%dY", xpoints, y);

#if 0
	sprintf(format, "%%-%ds", wid);
#endif
	fprintf(fp, "%s", str);
}

void PCLBorders(FILE *fp)
{
}

struct typefaces { char *typeface, *code; } TypeFaces[] = {
	{ "lineprinter",	"\033(s0T" },
	{ "courier",		"\033(s4099T" },
	{ "albertus",		"\033(s4362T" },
	{ "antique olive",	"\033(s4168T" },
	{ "clarendon",		"\033(s4140T" },
	{ "coronet",		"\033(s4116T" },
	{ "garamond antiqua",	"\033(s4197T" },
	{ "letter gothic",	"\033(s4102T" },
	{ "marigold",		"\033(s4297T" },
	{ "cg omega",		"\033(s4113T" },
	{ "cg times",		"\033(s4101T" },
	{ "univers",		"\033(s4148T" },
	{ "arial",		"\033(s16602T" },
	{ "times new roman",	"\033(s16901T" },
	{ "symbol",		"\033(s16686T" },
	{ "wingdings",		"\033(s31402T" },
	{ NULL, NULL }
};

void PCLFont(char *family, char *slant, int size, FILE *fp)
{
	int	i, found;

	/* Typeface */
	found = 0;
	for (i=0; TypeFaces[i].typeface; i++) {
		if (strstr(family, TypeFaces[i].typeface)) {
			fprintf(fp, "%s", TypeFaces[i].code);
			found = 1;
			break;
		}
	}

	if (found == 0)
		fprintf(fp, "%s", TypeFaces[0].code);

	/* Pitch - FIX ME from prls */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('s', fp);
#if 0
	fputc('2', fp);
	fputc('0', fp);
#else
	fprintf(fp, "%d", size * 2);
#endif
	fputc('H', fp);

	/* Primary size */
	/* Original version sent \033(s%dV,
	 * the www.wotsit.org docs say v instead of V.
	 */
	fputc('\033', fp);
	fputc('(', fp);
	fputc('s', fp);
	fprintf(fp, "%d", size);
	fputc('v', fp);

	/* Slant */
	if (slant == NULL || stricmp(slant, "normal") == 0
			|| stricmp(slant, "medium") == 0
			|| strlen(slant) ==0) {
		fputc('\033', fp);
		fputc('(', fp);
		fputc('s', fp);
		fputc('0', fp);
		fputc('S', fp);
		fputc('\033', fp);
		fputc('(', fp);
		fputc('s', fp);
		fputc('0', fp);
		fputc('B', fp);
	} else if (strstr(slant, "bold") != NULL) {	/* Bold */
		if (strstr(slant, "italic") != NULL) {	/* Bold-Italic */
			fputc('\033', fp);
			fputc('(', fp);
			fputc('s', fp);
			fputc('1', fp);
			fputc('S', fp);
		} else {
			fputc('\033', fp);
			fputc('(', fp);
			fputc('s', fp);
			fputc('0', fp);
			fputc('S', fp);
		}
			fputc('\033', fp);
			fputc('(', fp);
			fputc('s', fp);
			fputc('3', fp);
			fputc('B', fp);
	} else {	/* Not bold */
		if (strstr(slant, "italic") != NULL) {	/* Italic */
			fputc('\033', fp);
			fputc('(', fp);
			fputc('s', fp);
			fputc('1', fp);
			fputc('S', fp);
		} else {
			fputc('\033', fp);
			fputc('(', fp);
			fputc('s', fp);
			fputc('0', fp);
			fputc('S', fp);
		}
			fputc('\033', fp);
			fputc('(', fp);
			fputc('s', fp);
			fputc('0', fp);
			fputc('B', fp);
	}
}

void PCLNewLine(int ht, FILE *fp)
{
	fputc('\n', fp);
	fputc('\r', fp);

	y += ht;
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
	&PCLPaperSize
};

#ifdef	TEST
int main(int argc, char *argv[])
{
	struct PrintDriver *pd = &PCLPrintDriver;
	FILE			*fp = fopen("test.out", "w");

	fprintf(stderr, "Testing print driver for '%s'\n", pd->name);
	pd->job_header("This is a title", 1, fp);
	pd->font("times", "italic", 8, fp);
	pd->page_header("Page 1", fp);
	pd->field("Field 1", 10, 0, 1, fp);
	pd->font("times", "bold", 8, fp);
	pd->field("Field 2", 10, 0, 1, fp);
	pd->font("times", "bold-italic", 8, fp);
	pd->field("Field 3", 10, 0, 1, fp);

	pd->newline(8, fp);
	pd->font("cg times", NULL, 8, fp);
	pd->field("Field 4 - this is in CG Times", 40, 0, 1, fp);

	pd->newline(8, fp);
	pd->font("marigold", NULL, 8, fp);
	pd->field("Field 5 - this is in Marigold", 40, 0, 1, fp);

	pd->newline(8, fp);
	pd->font("clarendon", NULL, 8, fp);
	pd->field("Field 6 - this is in Clarendon", 40, 0, 1, fp);

	pd->newline(8, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 7 - this is in Letter Gothic", 60, 0, 1, fp);

	pd->newline(8, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 8 - centered in Letter Gothic", 60, JST_CNT, 1, fp);

	pd->newline(8, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 9 - right in Letter Gothic", 60, JST_RGT, 1, fp);

	pd->newline(8, fp);
	pd->font("letter gothic", NULL, 8, fp);
	pd->field("Field 10 - left in Letter Gothic", 60, JST_LFT, 1, fp);

	pd->page_footer("End page 1", fp);
	pd->job_trailer(1, fp);

	fclose(fp);

	exit(0);
}
#endif	/* TEST */
#if 0
struct PrintDriver {
	char	*name;
	void	(*job_header) (char *title, int npages, FILE *fp);
	void	(*job_trailer) (int npages, FILE *fp);

	void	(*page_header)(char *str, FILE *);
	void	(*page_footer)(char *str, FILE *);

	void	(*field)(char *str, int wid, int rightborder, FILE *);
	void	(*borders)(/* ... , */ FILE *);
	void	(*font)(char *family, char *slant, int size, FILE *);
	void	(*newline)(int ht, FILE *);
	void	(*paper_size)(int wid, int ht, FILE *);
};
#endif
