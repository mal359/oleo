/*
 *  $Id: appres.c,v 1.2 1998/08/28 15:30:39 danny Exp $
 *
 *  This file is part of Oleo, the GNU spreadsheet.
 *
 *  Copyright (c) 1998 by the Free Software Foundation, Inc.
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

static char rcsid[] = "$Id: appres.c,v 1.2 1998/08/28 15:30:39 danny Exp $";

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>

#include "appres.h"

XtResource resources[] = {
    {	"showVersion", "ShowVersion", XtRBoolean, sizeof(Boolean),
	XtOffsetOf(GnuSheetAppres, show_version), XtRString, False },
    {	"quiet", "Quiet", XtRBoolean, sizeof(Boolean),
	XtOffsetOf(GnuSheetAppres, quiet), XtRString, False },
    {	"ignoreInitFile", "IgnoreInitFile", XtRBoolean, sizeof(Boolean),
	XtOffsetOf(GnuSheetAppres, ignore_init_file), XtRString, False },
    {	"initFile", "InitFile", XtRString, sizeof(String),
	XtOffsetOf(GnuSheetAppres, init_file), XtRString, ".oleo" },
    {	"showUsage", "ShowUsage", XtRBoolean, sizeof(Boolean),
	XtOffsetOf(GnuSheetAppres, show_usage), XtRString, False },
    {	"rows", "Rows", XtRInt, sizeof(int),
	XtOffsetOf(GnuSheetAppres, rows), XtRImmediate, (XtPointer)255 },
    {	"columns", "columns", XtRInt, sizeof(int),
	XtOffsetOf(GnuSheetAppres, columns), XtRImmediate, (XtPointer)255 },
    {	"quitMessage", "QuitMessage", XmRXmString, sizeof(XmString),
	XtOffsetOf(GnuSheetAppres, quit_message), XtRString,
	(XtPointer)"There are unsaved changes.\nDo you want to quit anyway ?" },
    {	"saveAsTitle", "SaveAsTitle", XtRString, sizeof(String),
	XtOffsetOf(GnuSheetAppres, save_as_title), XtRString,
	(XtPointer)"Choose a file to save in" },
    {	"loadTitle", "LoadTitle", XtRString, sizeof(String),
	XtOffsetOf(GnuSheetAppres, load_title), XtRString,
	(XtPointer)"Choose a file to load" },
};

int num_resources = XtNumber(resources);
