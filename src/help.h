#ifndef HELPH
#define HELPH

/*	Copyright (C) 1993 Free Software Foundation, Inc.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this software; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.  */
/*  t. lord	Sun Feb 21 16:34:20 1993	*/

#include "info.h"


extern void expand_help_msg ();
extern void describe_function ();
extern void brief_describe_key ();
extern void describe_key ();
extern void where_is ();
extern void help_with_command ();
extern void builtin_help ();
extern void make_wallchart_info ();
extern void write_info ();

#endif  /* HELPH */
