/*
 * Copyright (C) 1999 Free Software Foundation, Inc.
 *
 * $Id: afm.c,v 1.2 1999/10/24 12:19:54 danny Exp $
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

/*
 * This is Oleo's AFM module.
 *
 * Its purpose is to locate and interpret AFM (Adobe Font Metric) files,
 * and to determine string lengths based on that information.
 *
 * The return value of AfmStringWidth is of course a number that represents
 * the string's width in points.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

#define	AFM_PATH	BUILD_PREFIX "/share/afm"

#define	LINE_LEN	128

static char	*CurrentFontName = NULL,
		*CurrentFontSlant = NULL;
static int	CurrentFontSize = 10;

struct afm {
	int	IsFixedPitch, ItalicAngle, FontBBox1, FontBBox2, FontBBox3, FontBBox4;
	int	UnderlinePosition, UnderlineThickness, CapHeight, XHeight, Ascender, Descender;
	int	StartCharMetrics;
};

static struct afm *afm = 0;

static int changed(char *a, char *b)
{
	if (a == 0 && b == 0)
		return 0;
	if (a != 0 && b != 0 && strcmp(a, b) == 0)
		return 0;
	return 1;
}

static char *Where, *line;

static int IsCommand(char *literal)
{
	int	len;

	len = strlen(literal);

	Where = line + len + 1;

	return (strncmp(literal, line, len) == 0);
}

static int Value()
{
	int	v;

	sscanf(Where, "%d", &v);
	return v;
}

static int ValueBoolean()
{
	if (strncmp(Where, "True", 4) == 0)
		return 1;
	if (strncmp(Where, "true", 4) == 0)
		return 1;
	if (strncmp(Where, "TRUE", 4) == 0)
		return 1;
	return 0;
}

static void
ReadAfmLine(void)
{
	if (afm == 0)
		afm = (struct afm *)malloc(sizeof(struct afm));

	if (IsCommand("IsFixedPitch")) ValueBoolean(&afm->IsFixedPitch);
	if (IsCommand("ItalicAngle")) Value(&afm->ItalicAngle);
	if (IsCommand("FontBBox"))
		sscanf(Where, "%d %d %d %d", &afm->FontBBox1, &afm->FontBBox2,
			&afm->FontBBox3, &afm->FontBBox4);
}

void
AfmSetFont(char *name, char *slant, int size)
{
	char	*fn;
	FILE	*fp;

/* Have the font name/slant changed ? */
	CurrentFontSize = size;

	if ( ( ! changed(name, CurrentFontName)) &&
		( ! changed(slant, CurrentFontSlant)))
		return;

/*
 * Something's changed, need to read another file.
 *
 * First however remember current state.
 */
	if (CurrentFontName) {
		free(CurrentFontName);
		CurrentFontName = 0;
	}
	if (CurrentFontSlant) {
		free(CurrentFontSlant);
		CurrentFontSlant = 0;
	}

	if (name)
		CurrentFontName = strdup(name);
	if (slant)
		CurrentFontSlant = strdup(slant);

/*
 * Start reading file.
 */
	fn = malloc(MAXPATHLEN);
	sprintf(fn, "%s/%s%s.afm", AFM_PATH, name, slant ? slant : "");
#if 1
	fprintf(stderr, "Try opening %s\n", fn);
#endif
	fp = fopen(fn, "r");

	if (fp == 0) {
		free(fn);
		return;
	}
	free(fn);

	line = malloc(LINE_LEN + 3);

	while (fgets(line, LINE_LEN, fp) != NULL) {
		ReadAfmLine();
	}

	fclose(fp);
	free(line);
}

int
AfmStringWidth(char *s)
{
	if (s == NULL || afm == 0)
		return 0;
	return strlen(s) * afm->FontBBox3 * CurrentFontSize / 1000;
}
