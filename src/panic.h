#ifndef PANICH
#define PANICH

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
/*  t. lord	Fri May  7 13:49:46 1993	*/

extern void panic_write_file (FILE *fp, struct rng *rng);
extern void panic_read_file (FILE *fp, int ismerge);
extern int panic_set_options (int set_opt, char *option);
extern void panic_show_options (void);

#endif  /* PANICH */
