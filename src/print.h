#ifndef PRINTH
#define PRINTH

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
/*  t. lord	Sun Jan  3 20:19:59 1993	*/


extern void put_ps_string (char *str, FILE *fp);
extern void psprint_region (FILE * fp, struct rng * rng, float wid, float hgt, char * font);
extern void set_page_size_cmd (char * whole_str);
extern void print_region_cmd (struct rng *rng, FILE *fp);

extern int PrintGetNumPageSizes(void);
extern char * PrintGetPageName(int index);
extern void PrintSetPageSize(float wid, float ht);
extern void PrintSetPage(char *page);

struct page_size 
{
    char *name;
    float wid;
    float hgt;
};

struct PrintDriver {
	char	*name;
	void	(*job_header) (char *title, int npages, FILE *fp);
	void	(*job_trailer) (FILE *);

	void	(*page_header)(char *str, FILE *);
	void	(*page_footer)(char *str, FILE *);

	void	(*field)(char *str, int wid, int justify, int rightborder, FILE *);
	void	(*borders)(/* ... , */ FILE *);
	void	(*font)(char *fn, FILE *);
	void	(*newline)(int ht, FILE *);
	int	(*printer_justifies)(void);
};

char *PrintGetType(int);
void PrintSetType(char *);
#endif /* PRINTH */
