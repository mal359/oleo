/*
 * Copyright (C) 1999 Free Software Foundation, Inc.
 *
 * $Id: afm.c,v 1.1 1999/10/23 13:39:29 danny Exp $
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
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <ctype.h>

void
AfmSetFont(char *name, char *slant, int size)
{
}

int
AfmStringWidth(char *s)
{
	if (s == NULL)
		return 0;
	return strlen(s) * 10;
}
