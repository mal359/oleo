/*
 *  $Id: mdi.c,v 1.2 1999/09/02 22:53:48 danny Exp $
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

static char rcsid[] = "$Id: mdi.c,v 1.2 1999/09/02 22:53:48 danny Exp $";

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "global.h"
#include "io-motif.h"

static int nglobals = 0;
static struct OleoGlobal	*globals = 0;

#define	NGLOBALS_INC	10

static void AllocateSubStructures(void)
{
	PlotInit();
	AllocateDatabaseGlobal();
	MotifGlobalInitialize();
	InitializeGlobals();
}

void
MdiInitialize(void)
{
	globals = (struct OleoGlobal *)
		calloc(NGLOBALS_INC, sizeof(struct OleoGlobal));
	nglobals = NGLOBALS_INC;

	Global = &globals[0];
	globals[0].valid = 1;

	AllocateSubStructures();
}

/*
 * MdiOpen()
 *
 * Create an additional Global structure
 */
void
MdiOpen()
{
	int	i, j, found = 0;

	for (i=0; i<nglobals; i++)
		if (globals[i].valid == 0) {
			found = 1;
			break;
		}
	/* If none free, allocate more entries */
	if (! found) {
		i = nglobals;
		nglobals += NGLOBALS_INC;
		globals = realloc(globals, nglobals * sizeof(struct OleoGlobal));
		for (j=i; j<nglobals; j++)
			globals[j].valid = 0;
	}

	/* Grab it */
	globals[i].valid = 1;

	/* Start using it */
	Global = &globals[i];

	/* Allocate the sub-structures */
	AllocateSubStructures();
}

/*
 * Close the current Global structure
 */
MdiClose()
{
	MessageAppend(False, "Closing file '%s'", Global->FileName);
}

void
MdiSelectGlobal(int offset1, int offset2, void *ptr)
{
	int	i;
	char	**p, **q;

	for (i=0; i < nglobals; i++)
		if (globals[i].valid) {
			p = (char **) (((char *)&globals[i]) + offset1);
			q = (char **) (*p + offset2);
			if (*q == ptr) {
				Global = &globals[i];
				return;
			}
		}
	MessageAppend(True,
		"MdiSelectGlobal: no match for offset %d,%d ptr %p\n",
		offset1, offset2, ptr);
}
