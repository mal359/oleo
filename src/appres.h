/*
 *  $Id: appres.h,v 1.1 1998/08/19 07:45:05 danny Exp $
 *
 *  This file is part of GnuSheet, a free spreadsheet.
 *
 *  Modifications from oleo-1.6 are copyright (C) 1998 by Danny Backx
 *  <danny.backx@advalvas.be>.
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
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>

typedef struct GnuSheetAppres {
	Boolean		show_version;
	Boolean		quiet;
	Boolean		ignore_init_file;
	String		init_file;
	Boolean		show_usage;
	int		rows, columns;
	XmString	quit_message;
	String		save_as_title, load_title;
} GnuSheetAppres;

extern XtResource	resources[];
extern int		num_resources;
extern GnuSheetAppres	AppRes;