/*
 *  $Id: xbase.cpp,v 1.2 1999/03/17 21:21:39 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright (C) 1998-1999 by the Free Software Foundation, Inc.
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

extern "C" {

static char rcsid[] = "$Id: xbase.cpp,v 1.2 1999/03/17 21:21:39 danny Exp $";

#include "config.h"

#ifdef	HAVE_LOCALE_H
#include <locale.h>
#endif

#include <libintl.h>

#include "cmd.h"
#include "basic.h"
#include "io-utils.h"
#include "io-term.h"

}

#if defined(HAVE_LIBXBASE)

#include <stdio.h>
#include <xbase/xbase.h>

void
CppReadXbaseFile(char *fn)
{
//	char	*fn;
	XBASE	x;
	DBF	db(&x);
	int	i, fc;
	char	*r;

//	fn = cell_value_string(curow, cucol, True);
//	fn = "/home/danny/LAND.DBF";

	db.OpenDatabase(fn);
	fc = db.FieldCount();

	for (i=0; i<fc; i++) {
		r = new_value(curow + 1, i + cucol,
			db.GetFieldName(i));
	}

	modified = 1;
	recalculate(1);
//	MotifUpdateDisplay();

	db.CloseDatabase();
}

extern "C" {
	void ReadXbaseFile(char *name, int ismerge)
	{
		CppReadXbaseFile(name);
	}
}

#ifdef	TEST
main()
{
	XBASE	x;
	DBF	db(&x);
	int	i, fc;

	db.OpenDatabase("ZIPCODES");

	fc = db.FieldCount();
	fprintf(stderr, "FieldCount %d\n", fc);

	for (i=0; i<fc; i++)
		fprintf(stderr, "Field %d '%s' length %d.%d type %c\n",
			i,
			db.GetFieldName(i),
			db.GetFieldLen(i),
			db.GetFieldDecimal(i),
			db.GetFieldType(i));
	db.CloseDatabase();
}
#endif	/* TEST */
#endif	/* HAVE_LIBXBASE */
