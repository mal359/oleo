/*
 *  $Id: io-motif.h,v 1.1 1998/08/19 07:45:07 danny Exp $
 *
 *  This file is part of GnuSheet, a free spreadsheet.
 *
 *  Copyright (C) 1998 by Danny Backx <danny.backx@advalvas.be>.
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

#include "config.h"

#include <Xm/Xm.h>

extern XtAppContext app;
extern Widget   toplevel;
extern Widget    splash, SplashShell;
extern Widget   mw, mat, mb, filemenu, editmenu, optionsmenu, helpmenu;
extern Widget   msgtext, statustf, formulatf;
extern Widget   fsd;

extern void versionCB(Widget, XtPointer, XtPointer);
extern void quitCB(Widget, XtPointer, XtPointer);
