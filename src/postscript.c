/*
 *  $Id: postscript.c,v 1.9 1999/09/19 09:26:31 danny Exp $
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

/*
 * This file contains the PostScript specific printing support.
 * It is called by src/print.c which contains general printing support.
 *
 * There shouldn't be much spreadsheet functionality here...
 */

static char rcsid[] = "$Id: postscript.c,v 1.9 1999/09/19 09:26:31 danny Exp $";

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
 * FIX ME these should be static
 */
extern float default_pswid, default_pshgt;

void 
put_ps_string (char *str, FILE *fp)
{
  fputc ('(', fp);
  while (*str)
    {
      if (*str == ')')
	fputs ("\\)", fp);
      else if (*str == '(')
	fputs ("\\(", fp);
      else
	fputc (*str, fp);
      ++str;
    }
  fputc (')', fp);
}

void PostScriptJobHeader(char *title, int npages, FILE *fp)
{
	struct font_names	*fn;
	struct font_memo	*fm;

	fprintf(fp, "%%!PS-Adobe-3.0\n");
	fprintf(fp, "%%%%Creator: %s %s\n", PACKAGE, VERSION);
	fprintf(fp, "%%%%Pages: (atend)\n");
	fprintf(fp, "%%%%PageOrder: Ascend\n");

	/*
	 * Print a list of all fonts
	 */
	fprintf(fp, "%%%%DocumentFonts:");
	for (fm = font_list; fm; fm = fm->next) {
		if ((fn = fm->names) == 0)
			continue;
		if (fn->ps_name == 0 || strlen(fn->ps_name) == 0)
			continue;
		fprintf(fp, " %sISO", fn->ps_name);
	}
	fprintf(fp, "\n");

	/* done */

	fprintf(fp, "%%%%Title: %s\n", title);
	fprintf(fp, "%%%%EndComments\n%%%%BeginProlog\n");

	/*
	 * Define a function to reencode all fonts to ISO Latin 1
	 */
	fprintf(fp, "/reencodeISO { %%def\n");
	fprintf(fp, "    findfont dup length dict begin\n");
	fprintf(fp, "    { 1 index /FID ne { def }{ pop pop } ifelse } forall\n");
	fprintf(fp, "    /Encoding ISOLatin1Encoding def\n");
	fprintf(fp, "    currentdict end definefont pop\n");
	fprintf(fp, "} bind def\n");

	/*
	 * Remap all used fonts to ISO, give them a new name.
	 */
	for (fm = font_list; fm; fm = fm->next) {
		if ((fn = fm->names) == 0)
			continue;
		if (fn->ps_name == 0 || strlen(fn->ps_name) == 0)
			continue;
		fprintf(fp, "/%sISO /%s reencodeISO\n", fn->ps_name, fn->ps_name);
	}

	/*
	 *
	 */
	fprintf(fp, "/FontName where { pop } { /FontName (Courier) def } ifelse\n");
	fprintf(fp, "/FirstSize where { pop } { /FirstSize 10 def } ifelse\n");
	fprintf(fp, "%%%%EndProlog\n");

}

void PostScriptJobTrailer(int npages, FILE *fp)
{
	fprintf(fp, "%%Pages: %d\n", npages);
	fprintf(fp, "%%EOF\n");
}

/*
 * These variables hold the position we're printing at
 */
static float	x, y;

#define	INITIAL_X	10.0
#define	INITIAL_Y	760.0
/*
 * 8 : too wide
 * 6 : no margin between columns
 * 7 : ??
 */
#define	MULTIPLY_WIDTH	7

void PostScriptPageHeader(char *str, FILE *fp)
{
	fprintf(fp, "%%%%Page: %s\n", str);
	x = INITIAL_X;
	y = INITIAL_Y;
}

void PostScriptPageFooter(char *str, FILE *fp)
{
	fprintf(fp, "showpage\n");
}

/*
 * Justify can have values JST_DEF, JST_LFT, JST_RGT, JST_CNT
 */
void PostScriptField(char *str, int wid, int justify, int rightborder, FILE *fp)
{
	float	w = MULTIPLY_WIDTH * wid;

	if (strlen(str)) {
		float	tw = strlen(str) * MULTIPLY_WIDTH;	/* Font size ?? */

		if (justify == JST_CNT) {
			fprintf(fp, "%3.1f %3.1f moveto ", x + (w - tw) / 2, y);
		} else if (justify == JST_RGT) {
			fprintf(fp, "%3.1f %3.1f moveto ", x + w - tw, y);
		} else {
			fprintf(fp, "%3.1f %3.1f moveto ", x, y);
		}

		put_ps_string(str, fp);
		fprintf(fp, " show\n");
	}

	x += w;

	if (rightborder) {
	}
}

void PostScriptBorders(FILE *fp)
{
}

void PostScriptFont(char *family, char *slant, int size, FILE *fp)
{
#if 0
	fprintf(fp, "/FontName where { pop } { /FontName (Palatino-Italic) def } ifelse\n");
	fprintf(fp, "/FirstSize where { pop } { /FirstSize 8 def } ifelse\n");
	fprintf(fp, "/FontName where { pop } { /FontName (Courier) def } ifelse\n");
	fprintf(fp, "/FirstSize where { pop } { /FirstSize 10 def } ifelse\n");
#endif
	fprintf(fp, "/%sISO findfont %d scalefont setfont\n", family, size);
#if 0
	fprintf(fp, "FontName findfont FirstSize scalefont setfont\n");
#endif
}

void PostScriptNewLine(int ht, FILE *fp)
{
	x = INITIAL_X;
	y -= ht;
}

int PostScriptPrinterJustifies(void)
{
	return 1;
}

void PostScriptPaperSize(int wid, int ht, FILE *fp)
{
}

struct PrintDriver PostScriptPrintDriver = {
	"PostScript",
	&PostScriptJobHeader,
	&PostScriptJobTrailer,
	&PostScriptPageHeader,
	&PostScriptPageFooter,
	&PostScriptField,
	&PostScriptBorders,
	&PostScriptFont,
	&PostScriptNewLine,
	&PostScriptPrinterJustifies,
	&PostScriptPaperSize
};
