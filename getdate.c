
/*  A Bison parser, made from /phydeaux/ni/lord/unsupported/oleo/getdate.y with Bison version GNU Bison version 1.22
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	tAGO	258
#define	tDAY	259
#define	tDAYZONE	260
#define	tID	261
#define	tMERIDIAN	262
#define	tMINUTE_UNIT	263
#define	tMONTH	264
#define	tMONTH_UNIT	265
#define	tSEC_UNIT	266
#define	tSNUMBER	267
#define	tUNUMBER	268
#define	tZONE	269
#define	tDST	270

#line 1 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"

/* $Revision: 1.1.1.1 $
**
**  Originally written by Steven M. Bellovin <smb@research.att.com> while
**  at the University of North Carolina at Chapel Hill.  Later tweaked by
**  a couple of people on Usenet.  Completely overhauled by Rich $alz
**  <rsalz@bbn.com> and Jim Berets <jberets@bbn.com> in August, 1990;
**  send any email to Rich.
**
**  This grammar has eight shift/reduce conflicts.
**
**  This code is in the public domain and has no copyright.
*/
/* SUPPRESS 287 on yaccpar_sccsid *//* Unusd static variable */
/* SUPPRESS 288 on gtderrlab *//* Label unused */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef __GNUC__
#define alloca __builtin_alloca
#else
#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#else
#ifdef _AIX /* for Bison */
 #pragma alloca
#else
char *alloca ();
#endif
#endif
#endif

#include <stdio.h>
#include <ctype.h>

/* The code at the top of get_date which figures out the offset of the
   current time zone checks various CPP symbols to see if special
   tricks are need, but defaults to using the gettimeofday system call.
   Include <sys/time.h> if that will be used.  */

#if !defined (USG) && !defined (sgi) && !defined (__386BSD__)
#include <sys/time.h>
#endif

#if	defined(vms)

#include <types.h>
#include <time.h>

#else

#include <sys/types.h>

#if	defined(USG) || !defined(HAVE_FTIME)
/*
**  If you need to do a tzset() call to set the
**  timezone, and don't have ftime().
*/
struct timeb {
    time_t		time;		/* Seconds since the epoch	*/
    unsigned short	millitm;	/* Field not used		*/
    short		timezone;
    short		dstflag;	/* Field not used		*/
};

#else

#include <sys/timeb.h>

#endif	/* defined(USG) && !defined(HAVE_FTIME) */

#if	defined(BSD4_2) || defined(BSD4_1C) || (defined (hp9000) && !defined (hpux))
#include <sys/time.h>
#else
#if defined(_AIX)
#include <sys/time.h>
#endif
#include <time.h>
#endif	/* defined(BSD4_2) */

#endif	/* defined(vms) */

#if defined (STDC_HEADERS) || defined (HAVE_STRING_H)
#include <string.h>
#endif

#if sgi
#undef timezone
#endif

extern struct tm	*localtime();

#define gtdparse getdate_gtdparse
#define gtdlex getdate_gtdlex
#define gtderror getdate_gtderror

#if	!defined(lint) && !defined(SABER)
static char RCS[] =
	"$Header: /home/matt/cvs/oleo/oleo/Attic/getdate.c,v 1.1.1.1 1997/06/01 17:34:51 jbailey Exp $";
#endif	/* !defined(lint) && !defined(SABER) */


#define EPOCH		1970
#define HOUR(x)		((time_t)(x) * 60)
#define SECSPERDAY	(24L * 60L * 60L)


/*
**  An entry in the lexical lookup table.
*/
typedef struct _TABLE {
    char	*name;
    int		type;
    time_t	value;
} TABLE;


/*
**  Daylight-savings mode:  on, off, or not yet known.
*/
typedef enum _DSTMODE {
    DSTon, DSToff, DSTmaybe
} DSTMODE;

/*
**  Meridian:  am, pm, or 24-hour style.
*/
typedef enum _MERIDIAN {
    MERam, MERpm, MER24
} MERIDIAN;


/*
**  Global variables.  We could get rid of most of these by using a good
**  union as the yacc stack.  (This routine was originally written before
**  yacc had the %union construct.)  Maybe someday; right now we only use
**  the %union very rarely.
*/
static char	*gtdInput;
static DSTMODE	gtdDSTmode;
static time_t	gtdDayOrdinal;
static time_t	gtdDayNumber;
static int	gtdHaveDate;
static int	gtdHaveDay;
static int	gtdHaveRel;
static int	gtdHaveTime;
static int	gtdHaveZone;
static time_t	gtdTimezone;
static time_t	gtdDay;
static time_t	gtdHour;
static time_t	gtdMinutes;
static time_t	gtdMonth;
static time_t	gtdSeconds;
static time_t	gtdYear;
static MERIDIAN	gtdMeridian;
static time_t	gtdRelMonth;
static time_t	gtdRelSeconds;


#line 163 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
typedef union {
    time_t		Number;
    enum _MERIDIAN	Meridian;
} YYSTYPE;

#ifndef YYLTYPE
typedef
  struct gtdltype
    {
      int timestamp;
      int first_line;
      int first_column;
      int last_line;
      int last_column;
      char *text;
   }
  gtdltype;

#define YYLTYPE gtdltype
#endif

#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		51
#define	YYFLAG		-32768
#define	YYNTBASE	19

#define YYTRANSLATE(x) ((unsigned)(x) <= 270 ? gtdtranslate[x] : 29)

static const char gtdtranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,    17,     2,     2,    18,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    16,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     2,     3,     4,     5,
     6,     7,     8,     9,    10,    11,    12,    13,    14,    15
};

#if YYDEBUG != 0
static const short gtdprhs[] = {     0,
     0,     1,     4,     6,     8,    10,    12,    14,    16,    19,
    24,    29,    36,    43,    45,    47,    50,    52,    55,    58,
    62,    68,    72,    75,    80,    83,    87,    90,    92,    95,
    98,   100,   103,   106,   108,   111,   114,   116,   118,   119
};

static const short gtdrhs[] = {    -1,
    19,    20,     0,    21,     0,    22,     0,    24,     0,    23,
     0,    25,     0,    27,     0,    13,     7,     0,    13,    16,
    13,    28,     0,    13,    16,    13,    12,     0,    13,    16,
    13,    16,    13,    28,     0,    13,    16,    13,    16,    13,
    12,     0,    14,     0,     5,     0,    14,    15,     0,     4,
     0,     4,    17,     0,    13,     4,     0,    13,    18,    13,
     0,    13,    18,    13,    18,    13,     0,    13,    12,    12,
     0,     9,    13,     0,     9,    13,    17,    13,     0,    13,
     9,     0,    13,     9,    13,     0,    26,     3,     0,    26,
     0,    13,     8,     0,    12,     8,     0,     8,     0,    12,
    11,     0,    13,    11,     0,    11,     0,    12,    10,     0,
    13,    10,     0,    10,     0,    13,     0,     0,     7,     0
};

#endif

#if YYDEBUG != 0
static const short gtdrline[] = { 0,
   177,   178,   181,   184,   187,   190,   193,   196,   199,   205,
   211,   218,   224,   234,   238,   242,   249,   253,   257,   263,
   267,   272,   278,   282,   287,   291,   298,   302,   305,   308,
   311,   314,   317,   320,   323,   326,   329,   334,   362,   365
};

static const char * const gtdtname[] = {   "$","error","$illegal.","tAGO","tDAY",
"tDAYZONE","tID","tMERIDIAN","tMINUTE_UNIT","tMONTH","tMONTH_UNIT","tSEC_UNIT",
"tSNUMBER","tUNUMBER","tZONE","tDST","':'","','","'/'","spec","item","time",
"zone","day","date","rel","relunit","number","o_merid",""
};
#endif

static const short gtdr1[] = {     0,
    19,    19,    20,    20,    20,    20,    20,    20,    21,    21,
    21,    21,    21,    22,    22,    22,    23,    23,    23,    24,
    24,    24,    24,    24,    24,    24,    25,    25,    26,    26,
    26,    26,    26,    26,    26,    26,    26,    27,    28,    28
};

static const short gtdr2[] = {     0,
     0,     2,     1,     1,     1,     1,     1,     1,     2,     4,
     4,     6,     6,     1,     1,     2,     1,     2,     2,     3,
     5,     3,     2,     4,     2,     3,     2,     1,     2,     2,
     1,     2,     2,     1,     2,     2,     1,     1,     0,     1
};

static const short gtddefact[] = {     1,
     0,    17,    15,    31,     0,    37,    34,     0,    38,    14,
     2,     3,     4,     6,     5,     7,    28,     8,    18,    23,
    30,    35,    32,    19,     9,    29,    25,    36,    33,     0,
     0,     0,    16,    27,     0,    26,    22,    39,    20,    24,
    40,    11,     0,    10,     0,    39,    21,    13,    12,     0,
     0
};

static const short gtddefgoto[] = {     1,
    11,    12,    13,    14,    15,    16,    17,    18,    44
};

static const short gtdpact[] = {-32768,
     0,   -15,-32768,-32768,   -10,-32768,-32768,    25,    11,    -8,
-32768,-32768,-32768,-32768,-32768,-32768,    13,-32768,-32768,     7,
-32768,-32768,-32768,-32768,-32768,-32768,     4,-32768,-32768,    14,
    15,    19,-32768,-32768,    24,-32768,-32768,    18,    20,-32768,
-32768,-32768,    26,-32768,    27,    -6,-32768,-32768,-32768,    31,
-32768
};

static const short gtdpgoto[] = {-32768,
-32768,-32768,-32768,-32768,-32768,-32768,-32768,-32768,    -5
};


#define	YYLAST		41


static const short gtdtable[] = {    50,
    41,    19,    20,     2,     3,    48,    33,     4,     5,     6,
     7,     8,     9,    10,    24,    34,    36,    25,    26,    27,
    28,    29,    30,    35,    41,    37,    31,    38,    32,    42,
    51,    39,    21,    43,    22,    23,    40,    45,    46,    47,
    49
};

static const short gtdcheck[] = {     0,
     7,    17,    13,     4,     5,    12,    15,     8,     9,    10,
    11,    12,    13,    14,     4,     3,    13,     7,     8,     9,
    10,    11,    12,    17,     7,    12,    16,    13,    18,    12,
     0,    13,     8,    16,    10,    11,    13,    18,    13,    13,
    46
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/unsupported/lib/bison.simple"

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Bob Corbett and Richard Stallman

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 1, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */


#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define gtderrok		(gtderrstatus = 0)
#define gtdclearin	(gtdchar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
#define YYERROR		goto gtderrlab1
/* Like YYERROR except do call gtderror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto gtderrlab
#define YYRECOVERING()  (!!gtderrstatus)
#define YYBACKUP(token, value) \
do								\
  if (gtdchar == YYEMPTY && gtdlen == 1)				\
    { gtdchar = (token), gtdlval = (value);			\
      gtdchar1 = YYTRANSLATE (gtdchar);				\
      YYPOPSTACK;						\
      goto gtdbackup;						\
    }								\
  else								\
    { gtderror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		gtdlex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#define YYLEX		gtdlex(&gtdlval, &gtdlloc)
#else
#define YYLEX		gtdlex(&gtdlval)
#endif
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	gtdchar;			/*  the lookahead symbol		*/
YYSTYPE	gtdlval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE gtdlloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int gtdnerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int gtddebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
int gtdparse (void);
#endif

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __gtd_bcopy(FROM,TO,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__gtd_bcopy (from, to, count)
     char *from;
     char *to;
     int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__gtd_bcopy (char *from, char *to, int count)
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 184 "/usr/unsupported/lib/bison.simple"
int
gtdparse()
{
  register int gtdstate;
  register int gtdn;
  register short *gtdssp;
  register YYSTYPE *gtdvsp;
  int gtderrstatus;	/*  number of tokens to shift before error messages enabled */
  int gtdchar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	gtdssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE gtdvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *gtdss = gtdssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *gtdvs = gtdvsa;	/*  to allow gtdoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE gtdlsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *gtdls = gtdlsa;
  YYLTYPE *gtdlsp;

#define YYPOPSTACK   (gtdvsp--, gtdssp--, gtdlsp--)
#else
#define YYPOPSTACK   (gtdvsp--, gtdssp--)
#endif

  int gtdstacksize = YYINITDEPTH;

#ifdef YYPURE
  int gtdchar;
  YYSTYPE gtdlval;
  int gtdnerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE gtdlloc;
#endif
#endif

  YYSTYPE gtdval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int gtdlen;

#if YYDEBUG != 0
  if (gtddebug)
    fprintf(stderr, "Starting parse\n");
#endif

  gtdstate = 0;
  gtderrstatus = 0;
  gtdnerrs = 0;
  gtdchar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  gtdssp = gtdss - 1;
  gtdvsp = gtdvs;
#ifdef YYLSP_NEEDED
  gtdlsp = gtdls;
#endif

/* Push a new state, which is found in  gtdstate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
gtdnewstate:

  *++gtdssp = gtdstate;

  if (gtdssp >= gtdss + gtdstacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *gtdvs1 = gtdvs;
      short *gtdss1 = gtdss;
#ifdef YYLSP_NEEDED
      YYLTYPE *gtdls1 = gtdls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = gtdssp - gtdss + 1;

#ifdef gtdoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if gtdoverflow is a macro.  */
      gtdoverflow("parser stack overflow",
		 &gtdss1, size * sizeof (*gtdssp),
		 &gtdvs1, size * sizeof (*gtdvsp),
		 &gtdls1, size * sizeof (*gtdlsp),
		 &gtdstacksize);
#else
      gtdoverflow("parser stack overflow",
		 &gtdss1, size * sizeof (*gtdssp),
		 &gtdvs1, size * sizeof (*gtdvsp),
		 &gtdstacksize);
#endif

      gtdss = gtdss1; gtdvs = gtdvs1;
#ifdef YYLSP_NEEDED
      gtdls = gtdls1;
#endif
#else /* no gtdoverflow */
      /* Extend the stack our own way.  */
      if (gtdstacksize >= YYMAXDEPTH)
	{
	  gtderror("parser stack overflow");
	  return 2;
	}
      gtdstacksize *= 2;
      if (gtdstacksize > YYMAXDEPTH)
	gtdstacksize = YYMAXDEPTH;
      gtdss = (short *) alloca (gtdstacksize * sizeof (*gtdssp));
      __gtd_bcopy ((char *)gtdss1, (char *)gtdss, size * sizeof (*gtdssp));
      gtdvs = (YYSTYPE *) alloca (gtdstacksize * sizeof (*gtdvsp));
      __gtd_bcopy ((char *)gtdvs1, (char *)gtdvs, size * sizeof (*gtdvsp));
#ifdef YYLSP_NEEDED
      gtdls = (YYLTYPE *) alloca (gtdstacksize * sizeof (*gtdlsp));
      __gtd_bcopy ((char *)gtdls1, (char *)gtdls, size * sizeof (*gtdlsp));
#endif
#endif /* no gtdoverflow */

      gtdssp = gtdss + size - 1;
      gtdvsp = gtdvs + size - 1;
#ifdef YYLSP_NEEDED
      gtdlsp = gtdls + size - 1;
#endif

#if YYDEBUG != 0
      if (gtddebug)
	fprintf(stderr, "Stack size increased to %d\n", gtdstacksize);
#endif

      if (gtdssp >= gtdss + gtdstacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (gtddebug)
    fprintf(stderr, "Entering state %d\n", gtdstate);
#endif

  goto gtdbackup;
 gtdbackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* gtdresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  gtdn = gtdpact[gtdstate];
  if (gtdn == YYFLAG)
    goto gtddefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* gtdchar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (gtdchar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (gtddebug)
	fprintf(stderr, "Reading a token: ");
#endif
      gtdchar = YYLEX;
    }

  /* Convert token to internal form (in gtdchar1) for indexing tables with */

  if (gtdchar <= 0)		/* This means end of input. */
    {
      gtdchar1 = 0;
      gtdchar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (gtddebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      gtdchar1 = YYTRANSLATE(gtdchar);

#if YYDEBUG != 0
      if (gtddebug)
	{
	  fprintf (stderr, "Next token is %d (%s", gtdchar, gtdtname[gtdchar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, gtdchar, gtdlval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  gtdn += gtdchar1;
  if (gtdn < 0 || gtdn > YYLAST || gtdcheck[gtdn] != gtdchar1)
    goto gtddefault;

  gtdn = gtdtable[gtdn];

  /* gtdn is what to do for this token type in this state.
     Negative => reduce, -gtdn is rule number.
     Positive => shift, gtdn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (gtdn < 0)
    {
      if (gtdn == YYFLAG)
	goto gtderrlab;
      gtdn = -gtdn;
      goto gtdreduce;
    }
  else if (gtdn == 0)
    goto gtderrlab;

  if (gtdn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (gtddebug)
    fprintf(stderr, "Shifting token %d (%s), ", gtdchar, gtdtname[gtdchar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (gtdchar != YYEOF)
    gtdchar = YYEMPTY;

  *++gtdvsp = gtdlval;
#ifdef YYLSP_NEEDED
  *++gtdlsp = gtdlloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (gtderrstatus) gtderrstatus--;

  gtdstate = gtdn;
  goto gtdnewstate;

/* Do the default action for the current state.  */
gtddefault:

  gtdn = gtddefact[gtdstate];
  if (gtdn == 0)
    goto gtderrlab;

/* Do a reduction.  gtdn is the number of a rule to reduce with.  */
gtdreduce:
  gtdlen = gtdr2[gtdn];
  if (gtdlen > 0)
    gtdval = gtdvsp[1-gtdlen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (gtddebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       gtdn, gtdrline[gtdn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = gtdprhs[gtdn]; gtdrhs[i] > 0; i++)
	fprintf (stderr, "%s ", gtdtname[gtdrhs[i]]);
      fprintf (stderr, " -> %s\n", gtdtname[gtdr1[gtdn]]);
    }
#endif


  switch (gtdn) {

case 3:
#line 181 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHaveTime++;
	;
    break;}
case 4:
#line 184 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHaveZone++;
	;
    break;}
case 5:
#line 187 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHaveDate++;
	;
    break;}
case 6:
#line 190 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHaveDay++;
	;
    break;}
case 7:
#line 193 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHaveRel++;
	;
    break;}
case 9:
#line 199 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHour = gtdvsp[-1].Number;
	    gtdMinutes = 0;
	    gtdSeconds = 0;
	    gtdMeridian = gtdvsp[0].Meridian;
	;
    break;}
case 10:
#line 205 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHour = gtdvsp[-3].Number;
	    gtdMinutes = gtdvsp[-1].Number;
	    gtdSeconds = 0;
	    gtdMeridian = gtdvsp[0].Meridian;
	;
    break;}
case 11:
#line 211 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHour = gtdvsp[-3].Number;
	    gtdMinutes = gtdvsp[-1].Number;
	    gtdMeridian = MER24;
	    gtdDSTmode = DSToff;
	    gtdTimezone = - (gtdvsp[0].Number % 100 + (gtdvsp[0].Number / 100) * 60);
	;
    break;}
case 12:
#line 218 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHour = gtdvsp[-5].Number;
	    gtdMinutes = gtdvsp[-3].Number;
	    gtdSeconds = gtdvsp[-1].Number;
	    gtdMeridian = gtdvsp[0].Meridian;
	;
    break;}
case 13:
#line 224 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdHour = gtdvsp[-5].Number;
	    gtdMinutes = gtdvsp[-3].Number;
	    gtdSeconds = gtdvsp[-1].Number;
	    gtdMeridian = MER24;
	    gtdDSTmode = DSToff;
	    gtdTimezone = - (gtdvsp[0].Number % 100 + (gtdvsp[0].Number / 100) * 60);
	;
    break;}
case 14:
#line 234 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdTimezone = gtdvsp[0].Number;
	    gtdDSTmode = DSToff;
	;
    break;}
case 15:
#line 238 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdTimezone = gtdvsp[0].Number;
	    gtdDSTmode = DSTon;
	;
    break;}
case 16:
#line 243 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdTimezone = gtdvsp[-1].Number;
	    gtdDSTmode = DSTon;
	;
    break;}
case 17:
#line 249 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdDayOrdinal = 1;
	    gtdDayNumber = gtdvsp[0].Number;
	;
    break;}
case 18:
#line 253 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdDayOrdinal = 1;
	    gtdDayNumber = gtdvsp[-1].Number;
	;
    break;}
case 19:
#line 257 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdDayOrdinal = gtdvsp[-1].Number;
	    gtdDayNumber = gtdvsp[0].Number;
	;
    break;}
case 20:
#line 263 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdMonth = gtdvsp[-2].Number;
	    gtdDay = gtdvsp[0].Number;
	;
    break;}
case 21:
#line 267 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdMonth = gtdvsp[-4].Number;
	    gtdDay = gtdvsp[-2].Number;
	    gtdYear = gtdvsp[0].Number;
	;
    break;}
case 22:
#line 272 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    /* ISO 8601 format.  gtdgtd-mm-dd.  */
	    gtdYear = gtdvsp[-2].Number;
	    gtdMonth = -gtdvsp[-1].Number;
	    gtdDay = -gtdvsp[0].Number;
	;
    break;}
case 23:
#line 278 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdMonth = gtdvsp[-1].Number;
	    gtdDay = gtdvsp[0].Number;
	;
    break;}
case 24:
#line 282 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdMonth = gtdvsp[-3].Number;
	    gtdDay = gtdvsp[-2].Number;
	    gtdYear = gtdvsp[0].Number;
	;
    break;}
case 25:
#line 287 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdMonth = gtdvsp[0].Number;
	    gtdDay = gtdvsp[-1].Number;
	;
    break;}
case 26:
#line 291 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdMonth = gtdvsp[-1].Number;
	    gtdDay = gtdvsp[-2].Number;
	    gtdYear = gtdvsp[0].Number;
	;
    break;}
case 27:
#line 298 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds = -gtdRelSeconds;
	    gtdRelMonth = -gtdRelMonth;
	;
    break;}
case 29:
#line 305 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds += gtdvsp[-1].Number * gtdvsp[0].Number * 60L;
	;
    break;}
case 30:
#line 308 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds += gtdvsp[-1].Number * gtdvsp[0].Number * 60L;
	;
    break;}
case 31:
#line 311 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds += gtdvsp[0].Number * 60L;
	;
    break;}
case 32:
#line 314 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds += gtdvsp[-1].Number;
	;
    break;}
case 33:
#line 317 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds += gtdvsp[-1].Number;
	;
    break;}
case 34:
#line 320 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelSeconds++;
	;
    break;}
case 35:
#line 323 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelMonth += gtdvsp[-1].Number * gtdvsp[0].Number;
	;
    break;}
case 36:
#line 326 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelMonth += gtdvsp[-1].Number * gtdvsp[0].Number;
	;
    break;}
case 37:
#line 329 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdRelMonth += gtdvsp[0].Number;
	;
    break;}
case 38:
#line 334 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    if (gtdHaveTime && gtdHaveDate && !gtdHaveRel)
		gtdYear = gtdvsp[0].Number;
	    else {
		if(gtdvsp[0].Number>10000) {
		    time_t date_part;

		    date_part= gtdvsp[0].Number/10000;
		    gtdHaveDate++;
		    gtdDay= (date_part)%100;
		    gtdMonth= (date_part/100)%100;
		    gtdYear = date_part/10000;
		} 
	        gtdHaveTime++;
		if (gtdvsp[0].Number < 100) {
		    gtdHour = gtdvsp[0].Number;
		    gtdMinutes = 0;
		}
		else {
		    gtdHour = gtdvsp[0].Number / 100;
		    gtdMinutes = gtdvsp[0].Number % 100;
		}
		gtdSeconds = 0;
		gtdMeridian = MER24;
	    }
	;
    break;}
case 39:
#line 362 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdval.Meridian = MER24;
	;
    break;}
case 40:
#line 365 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"
{
	    gtdval.Meridian = gtdvsp[0].Meridian;
	;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 465 "/usr/unsupported/lib/bison.simple"

  gtdvsp -= gtdlen;
  gtdssp -= gtdlen;
#ifdef YYLSP_NEEDED
  gtdlsp -= gtdlen;
#endif

#if YYDEBUG != 0
  if (gtddebug)
    {
      short *ssp1 = gtdss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != gtdssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++gtdvsp = gtdval;

#ifdef YYLSP_NEEDED
  gtdlsp++;
  if (gtdlen == 0)
    {
      gtdlsp->first_line = gtdlloc.first_line;
      gtdlsp->first_column = gtdlloc.first_column;
      gtdlsp->last_line = (gtdlsp-1)->last_line;
      gtdlsp->last_column = (gtdlsp-1)->last_column;
      gtdlsp->text = 0;
    }
  else
    {
      gtdlsp->last_line = (gtdlsp+gtdlen-1)->last_line;
      gtdlsp->last_column = (gtdlsp+gtdlen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  gtdn = gtdr1[gtdn];

  gtdstate = gtdpgoto[gtdn - YYNTBASE] + *gtdssp;
  if (gtdstate >= 0 && gtdstate <= YYLAST && gtdcheck[gtdstate] == *gtdssp)
    gtdstate = gtdtable[gtdstate];
  else
    gtdstate = gtddefgoto[gtdn - YYNTBASE];

  goto gtdnewstate;

gtderrlab:   /* here on detecting error */

  if (! gtderrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++gtdnerrs;

#ifdef YYERROR_VERBOSE
      gtdn = gtdpact[gtdstate];

      if (gtdn > YYFLAG && gtdn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -gtdn if nec to avoid negative indexes in gtdcheck.  */
	  for (x = (gtdn < 0 ? -gtdn : 0);
	       x < (sizeof(gtdtname) / sizeof(char *)); x++)
	    if (gtdcheck[x + gtdn] == x)
	      size += strlen(gtdtname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (gtdn < 0 ? -gtdn : 0);
		       x < (sizeof(gtdtname) / sizeof(char *)); x++)
		    if (gtdcheck[x + gtdn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, gtdtname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      gtderror(msg);
	      free(msg);
	    }
	  else
	    gtderror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	gtderror("parse error");
    }

  goto gtderrlab1;
gtderrlab1:   /* here on error raised explicitly by an action */

  if (gtderrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (gtdchar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (gtddebug)
	fprintf(stderr, "Discarding token %d (%s).\n", gtdchar, gtdtname[gtdchar1]);
#endif

      gtdchar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  gtderrstatus = 3;		/* Each real token shifted decrements this */

  goto gtderrhandle;

gtderrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  gtdn = gtddefact[gtdstate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (gtdn) goto gtddefault;
#endif

gtderrpop:   /* pop the current state because it cannot handle the error token */

  if (gtdssp == gtdss) YYABORT;
  gtdvsp--;
  gtdstate = *--gtdssp;
#ifdef YYLSP_NEEDED
  gtdlsp--;
#endif

#if YYDEBUG != 0
  if (gtddebug)
    {
      short *ssp1 = gtdss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != gtdssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

gtderrhandle:

  gtdn = gtdpact[gtdstate];
  if (gtdn == YYFLAG)
    goto gtderrdefault;

  gtdn += YYTERROR;
  if (gtdn < 0 || gtdn > YYLAST || gtdcheck[gtdn] != YYTERROR)
    goto gtderrdefault;

  gtdn = gtdtable[gtdn];
  if (gtdn < 0)
    {
      if (gtdn == YYFLAG)
	goto gtderrpop;
      gtdn = -gtdn;
      goto gtdreduce;
    }
  else if (gtdn == 0)
    goto gtderrpop;

  if (gtdn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (gtddebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++gtdvsp = gtdlval;
#ifdef YYLSP_NEEDED
  *++gtdlsp = gtdlloc;
#endif

  gtdstate = gtdn;
  goto gtdnewstate;
}
#line 370 "/phydeaux/ni/lord/unsupported/oleo/getdate.y"


/* Month and day table. */
static TABLE const MonthDayTable[] = {
    { "january",	tMONTH,  1 },
    { "february",	tMONTH,  2 },
    { "march",		tMONTH,  3 },
    { "april",		tMONTH,  4 },
    { "may",		tMONTH,  5 },
    { "june",		tMONTH,  6 },
    { "july",		tMONTH,  7 },
    { "august",		tMONTH,  8 },
    { "september",	tMONTH,  9 },
    { "sept",		tMONTH,  9 },
    { "october",	tMONTH, 10 },
    { "november",	tMONTH, 11 },
    { "december",	tMONTH, 12 },
    { "sunday",		tDAY, 0 },
    { "monday",		tDAY, 1 },
    { "tuesday",	tDAY, 2 },
    { "tues",		tDAY, 2 },
    { "wednesday",	tDAY, 3 },
    { "wednes",		tDAY, 3 },
    { "thursday",	tDAY, 4 },
    { "thur",		tDAY, 4 },
    { "thurs",		tDAY, 4 },
    { "friday",		tDAY, 5 },
    { "saturday",	tDAY, 6 },
    { NULL }
};

/* Time units table. */
static TABLE const UnitsTable[] = {
    { "year",		tMONTH_UNIT,	12 },
    { "month",		tMONTH_UNIT,	1 },
    { "fortnight",	tMINUTE_UNIT,	14 * 24 * 60 },
    { "week",		tMINUTE_UNIT,	7 * 24 * 60 },
    { "day",		tMINUTE_UNIT,	1 * 24 * 60 },
    { "hour",		tMINUTE_UNIT,	60 },
    { "minute",		tMINUTE_UNIT,	1 },
    { "min",		tMINUTE_UNIT,	1 },
    { "second",		tSEC_UNIT,	1 },
    { "sec",		tSEC_UNIT,	1 },
    { NULL }
};

/* Assorted relative-time words. */
static TABLE const OtherTable[] = {
    { "tomorrow",	tMINUTE_UNIT,	1 * 24 * 60 },
    { "yesterday",	tMINUTE_UNIT,	-1 * 24 * 60 },
    { "today",		tMINUTE_UNIT,	0 },
    { "now",		tMINUTE_UNIT,	0 },
    { "last",		tUNUMBER,	-1 },
    { "this",		tMINUTE_UNIT,	0 },
    { "next",		tUNUMBER,	2 },
    { "first",		tUNUMBER,	1 },
/*  { "second",		tUNUMBER,	2 }, */
    { "third",		tUNUMBER,	3 },
    { "fourth",		tUNUMBER,	4 },
    { "fifth",		tUNUMBER,	5 },
    { "sixth",		tUNUMBER,	6 },
    { "seventh",	tUNUMBER,	7 },
    { "eighth",		tUNUMBER,	8 },
    { "ninth",		tUNUMBER,	9 },
    { "tenth",		tUNUMBER,	10 },
    { "eleventh",	tUNUMBER,	11 },
    { "twelfth",	tUNUMBER,	12 },
    { "ago",		tAGO,	1 },
    { NULL }
};

/* The timezone table. */
/* Some of these are commented out because a time_t can't store a float. */
static TABLE const TimezoneTable[] = {
    { "gmt",	tZONE,     HOUR( 0) },	/* Greenwich Mean */
    { "ut",	tZONE,     HOUR( 0) },	/* Universal (Coordinated) */
    { "utc",	tZONE,     HOUR( 0) },
    { "wet",	tZONE,     HOUR( 0) },	/* Western European */
    { "bst",	tDAYZONE,  HOUR( 0) },	/* British Summer */
    { "wat",	tZONE,     HOUR( 1) },	/* West Africa */
    { "at",	tZONE,     HOUR( 2) },	/* Azores */
#if	0
    /* For completeness.  BST is also British Summer, and GST is
     * also Guam Standard. */
    { "bst",	tZONE,     HOUR( 3) },	/* Brazil Standard */
    { "gst",	tZONE,     HOUR( 3) },	/* Greenland Standard */
#endif
#if 0
    { "nft",	tZONE,     HOUR(3.5) },	/* Newfoundland */
    { "nst",	tZONE,     HOUR(3.5) },	/* Newfoundland Standard */
    { "ndt",	tDAYZONE,  HOUR(3.5) },	/* Newfoundland Daylight */
#endif
    { "ast",	tZONE,     HOUR( 4) },	/* Atlantic Standard */
    { "adt",	tDAYZONE,  HOUR( 4) },	/* Atlantic Daylight */
    { "est",	tZONE,     HOUR( 5) },	/* Eastern Standard */
    { "edt",	tDAYZONE,  HOUR( 5) },	/* Eastern Daylight */
    { "cst",	tZONE,     HOUR( 6) },	/* Central Standard */
    { "cdt",	tDAYZONE,  HOUR( 6) },	/* Central Daylight */
    { "mst",	tZONE,     HOUR( 7) },	/* Mountain Standard */
    { "mdt",	tDAYZONE,  HOUR( 7) },	/* Mountain Daylight */
    { "pst",	tZONE,     HOUR( 8) },	/* Pacific Standard */
    { "pdt",	tDAYZONE,  HOUR( 8) },	/* Pacific Daylight */
    { "yst",	tZONE,     HOUR( 9) },	/* Yukon Standard */
    { "ydt",	tDAYZONE,  HOUR( 9) },	/* Yukon Daylight */
    { "hst",	tZONE,     HOUR(10) },	/* Hawaii Standard */
    { "hdt",	tDAYZONE,  HOUR(10) },	/* Hawaii Daylight */
    { "cat",	tZONE,     HOUR(10) },	/* Central Alaska */
    { "ahst",	tZONE,     HOUR(10) },	/* Alaska-Hawaii Standard */
    { "nt",	tZONE,     HOUR(11) },	/* Nome */
    { "idlw",	tZONE,     HOUR(12) },	/* International Date Line West */
    { "cet",	tZONE,     -HOUR(1) },	/* Central European */
    { "met",	tZONE,     -HOUR(1) },	/* Middle European */
    { "mewt",	tZONE,     -HOUR(1) },	/* Middle European Winter */
    { "mest",	tDAYZONE,  -HOUR(1) },	/* Middle European Summer */
    { "swt",	tZONE,     -HOUR(1) },	/* Swedish Winter */
    { "sst",	tDAYZONE,  -HOUR(1) },	/* Swedish Summer */
    { "fwt",	tZONE,     -HOUR(1) },	/* French Winter */
    { "fst",	tDAYZONE,  -HOUR(1) },	/* French Summer */
    { "eet",	tZONE,     -HOUR(2) },	/* Eastern Europe, USSR Zone 1 */
    { "bt",	tZONE,     -HOUR(3) },	/* Baghdad, USSR Zone 2 */
#if 0
    { "it",	tZONE,     -HOUR(3.5) },/* Iran */
#endif
    { "zp4",	tZONE,     -HOUR(4) },	/* USSR Zone 3 */
    { "zp5",	tZONE,     -HOUR(5) },	/* USSR Zone 4 */
#if 0
    { "ist",	tZONE,     -HOUR(5.5) },/* Indian Standard */
#endif
    { "zp6",	tZONE,     -HOUR(6) },	/* USSR Zone 5 */
#if	0
    /* For completeness.  NST is also Newfoundland Stanard, and SST is
     * also Swedish Summer. */
    { "nst",	tZONE,     -HOUR(6.5) },/* North Sumatra */
    { "sst",	tZONE,     -HOUR(7) },	/* South Sumatra, USSR Zone 6 */
#endif	/* 0 */
    { "wast",	tZONE,     -HOUR(7) },	/* West Australian Standard */
    { "wadt",	tDAYZONE,  -HOUR(7) },	/* West Australian Daylight */
#if 0
    { "jt",	tZONE,     -HOUR(7.5) },/* Java (3pm in Cronusland!) */
#endif
    { "cct",	tZONE,     -HOUR(8) },	/* China Coast, USSR Zone 7 */
    { "jst",	tZONE,     -HOUR(9) },	/* Japan Standard, USSR Zone 8 */
#if 0
    { "cast",	tZONE,     -HOUR(9.5) },/* Central Australian Standard */
    { "cadt",	tDAYZONE,  -HOUR(9.5) },/* Central Australian Daylight */
#endif
    { "east",	tZONE,     -HOUR(10) },	/* Eastern Australian Standard */
    { "eadt",	tDAYZONE,  -HOUR(10) },	/* Eastern Australian Daylight */
    { "gst",	tZONE,     -HOUR(10) },	/* Guam Standard, USSR Zone 9 */
    { "nzt",	tZONE,     -HOUR(12) },	/* New Zealand */
    { "nzst",	tZONE,     -HOUR(12) },	/* New Zealand Standard */
    { "nzdt",	tDAYZONE,  -HOUR(12) },	/* New Zealand Daylight */
    { "idle",	tZONE,     -HOUR(12) },	/* International Date Line East */
    {  NULL  }
};

/* Military timezone table. */
static TABLE const MilitaryTable[] = {
    { "a",	tZONE,	HOUR(  1) },
    { "b",	tZONE,	HOUR(  2) },
    { "c",	tZONE,	HOUR(  3) },
    { "d",	tZONE,	HOUR(  4) },
    { "e",	tZONE,	HOUR(  5) },
    { "f",	tZONE,	HOUR(  6) },
    { "g",	tZONE,	HOUR(  7) },
    { "h",	tZONE,	HOUR(  8) },
    { "i",	tZONE,	HOUR(  9) },
    { "k",	tZONE,	HOUR( 10) },
    { "l",	tZONE,	HOUR( 11) },
    { "m",	tZONE,	HOUR( 12) },
    { "n",	tZONE,	HOUR(- 1) },
    { "o",	tZONE,	HOUR(- 2) },
    { "p",	tZONE,	HOUR(- 3) },
    { "q",	tZONE,	HOUR(- 4) },
    { "r",	tZONE,	HOUR(- 5) },
    { "s",	tZONE,	HOUR(- 6) },
    { "t",	tZONE,	HOUR(- 7) },
    { "u",	tZONE,	HOUR(- 8) },
    { "v",	tZONE,	HOUR(- 9) },
    { "w",	tZONE,	HOUR(-10) },
    { "x",	tZONE,	HOUR(-11) },
    { "y",	tZONE,	HOUR(-12) },
    { "z",	tZONE,	HOUR(  0) },
    { NULL }
};




/* ARGSUSED */
static int
gtderror(s)
    char	*s;
{
  return 0;
}


static time_t
ToSeconds(Hours, Minutes, Seconds, Meridian)
    time_t	Hours;
    time_t	Minutes;
    time_t	Seconds;
    MERIDIAN	Meridian;
{
    if (Minutes < 0 || Minutes > 59 || Seconds < 0 || Seconds > 59)
	return -1;
    switch (Meridian) {
    case MER24:
	if (Hours < 0 || Hours > 23)
	    return -1;
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERam:
	if (Hours < 1 || Hours > 12)
	    return -1;
	return (Hours * 60L + Minutes) * 60L + Seconds;
    case MERpm:
	if (Hours < 1 || Hours > 12)
	    return -1;
	return ((Hours + 12) * 60L + Minutes) * 60L + Seconds;
    }
    /* NOTREACHED */
}


static time_t
Convert(Month, Day, Year, Hours, Minutes, Seconds, Meridian, DSTmode)
    time_t	Month;
    time_t	Day;
    time_t	Year;
    time_t	Hours;
    time_t	Minutes;
    time_t	Seconds;
    MERIDIAN	Meridian;
    DSTMODE	DSTmode;
{
    static int DaysInMonth[12] = {
	31, 0, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    time_t	tod;
    time_t	Julian;
    int		i;

    if (Year < 0)
	Year = -Year;
    if (Year < 100)
	Year += 1900;
    DaysInMonth[1] = Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0)
		    ? 29 : 28;
    if (Year < EPOCH || Year > 1999
     || Month < 1 || Month > 12
     /* Lint fluff:  "conversion from long may lose accuracy" */
     || Day < 1 || Day > DaysInMonth[(int)--Month])
	return -1;

    for (Julian = Day - 1, i = 0; i < Month; i++)
	Julian += DaysInMonth[i];
    for (i = EPOCH; i < Year; i++)
	Julian += 365 + (i % 4 == 0);
    Julian *= SECSPERDAY;
    Julian += gtdTimezone * 60L;
    if ((tod = ToSeconds(Hours, Minutes, Seconds, Meridian)) < 0)
	return -1;
    Julian += tod;
    if (DSTmode == DSTon
     || (DSTmode == DSTmaybe && localtime(&Julian)->tm_isdst))
	Julian -= 60 * 60;
    return Julian;
}


static time_t
DSTcorrect(Start, Future)
    time_t	Start;
    time_t	Future;
{
    time_t	StartDay;
    time_t	FutureDay;

    StartDay = (localtime(&Start)->tm_hour + 1) % 24;
    FutureDay = (localtime(&Future)->tm_hour + 1) % 24;
    return (Future - Start) + (StartDay - FutureDay) * 60L * 60L;
}


static time_t
RelativeDate(Start, DayOrdinal, DayNumber)
    time_t	Start;
    time_t	DayOrdinal;
    time_t	DayNumber;
{
    struct tm	*tm;
    time_t	now;

    now = Start;
    tm = localtime(&now);
    now += SECSPERDAY * ((DayNumber - tm->tm_wday + 7) % 7);
    now += 7 * SECSPERDAY * (DayOrdinal <= 0 ? DayOrdinal : DayOrdinal - 1);
    return DSTcorrect(Start, now);
}


static time_t
RelativeMonth(Start, RelMonth)
    time_t	Start;
    time_t	RelMonth;
{
    struct tm	*tm;
    time_t	Month;
    time_t	Year;

    if (RelMonth == 0)
	return 0;
    tm = localtime(&Start);
    Month = 12 * tm->tm_year + tm->tm_mon + RelMonth;
    Year = Month / 12;
    Month = Month % 12 + 1;
    return DSTcorrect(Start,
	    Convert(Month, (time_t)tm->tm_mday, Year,
		(time_t)tm->tm_hour, (time_t)tm->tm_min, (time_t)tm->tm_sec,
		MER24, DSTmaybe));
}


static int
LookupWord(buff)
    char		*buff;
{
    register char	*p;
    register char	*q;
    register const TABLE	*tp;
    int			i;
    int			abbrev;

    /* Make it lowercase. */
    for (p = buff; *p; p++)
	if (isupper(*p))
	    *p = tolower(*p);

    if (strcmp(buff, "am") == 0 || strcmp(buff, "a.m.") == 0) {
	gtdlval.Meridian = MERam;
	return tMERIDIAN;
    }
    if (strcmp(buff, "pm") == 0 || strcmp(buff, "p.m.") == 0) {
	gtdlval.Meridian = MERpm;
	return tMERIDIAN;
    }

    /* See if we have an abbreviation for a month. */
    if (strlen(buff) == 3)
	abbrev = 1;
    else if (strlen(buff) == 4 && buff[3] == '.') {
	abbrev = 1;
	buff[3] = '\0';
    }
    else
	abbrev = 0;

    for (tp = MonthDayTable; tp->name; tp++) {
	if (abbrev) {
	    if (strncmp(buff, tp->name, 3) == 0) {
		gtdlval.Number = tp->value;
		return tp->type;
	    }
	}
	else if (strcmp(buff, tp->name) == 0) {
	    gtdlval.Number = tp->value;
	    return tp->type;
	}
    }

    for (tp = TimezoneTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    gtdlval.Number = tp->value;
	    return tp->type;
	}

    if (strcmp(buff, "dst") == 0) 
	return tDST;

    for (tp = UnitsTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    gtdlval.Number = tp->value;
	    return tp->type;
	}

    /* Strip off any plural and try the units table again. */
    i = strlen(buff) - 1;
    if (buff[i] == 's') {
	buff[i] = '\0';
	for (tp = UnitsTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		gtdlval.Number = tp->value;
		return tp->type;
	    }
	buff[i] = 's';		/* Put back for "this" in OtherTable. */
    }

    for (tp = OtherTable; tp->name; tp++)
	if (strcmp(buff, tp->name) == 0) {
	    gtdlval.Number = tp->value;
	    return tp->type;
	}

    /* Military timezones. */
    if (buff[1] == '\0' && isalpha(*buff)) {
	for (tp = MilitaryTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		gtdlval.Number = tp->value;
		return tp->type;
	    }
    }

    /* Drop out any periods and try the timezone table again. */
    for (i = 0, p = q = buff; *q; q++)
	if (*q != '.')
	    *p++ = *q;
	else
	    i++;
    *p = '\0';
    if (i)
	for (tp = TimezoneTable; tp->name; tp++)
	    if (strcmp(buff, tp->name) == 0) {
		gtdlval.Number = tp->value;
		return tp->type;
	    }

    return tID;
}


static int
gtdlex()
{
    register char	c;
    register char	*p;
    char		buff[20];
    int			Count;
    int			sign;

    for ( ; ; ) {
	while (isspace(*gtdInput))
	    gtdInput++;

	if (isdigit(c = *gtdInput) || c == '-' || c == '+') {
	    if (c == '-' || c == '+') {
		sign = c == '-' ? -1 : 1;
		if (!isdigit(*++gtdInput))
		    /* skip the '-' sign */
		    continue;
	    }
	    else
		sign = 0;
	    for (gtdlval.Number = 0; isdigit(c = *gtdInput++); )
		gtdlval.Number = 10 * gtdlval.Number + c - '0';
	    gtdInput--;
	    if (sign < 0)
		gtdlval.Number = -gtdlval.Number;
	    return sign ? tSNUMBER : tUNUMBER;
	}
	if (isalpha(c)) {
	    for (p = buff; isalpha(c = *gtdInput++) || c == '.'; )
		if (p < &buff[sizeof buff - 1])
		    *p++ = c;
	    *p = '\0';
	    gtdInput--;
	    return LookupWord(buff);
	}
	if (c != '(')
	    return *gtdInput++;
	Count = 0;
	do {
	    c = *gtdInput++;
	    if (c == '\0')
		return c;
	    if (c == '(')
		Count++;
	    else if (c == ')')
		Count--;
	} while (Count > 0);
    }
}


time_t
get_date(p, now)
    char		*p;
    struct timeb	*now;
{
    struct tm		*tm;
    struct timeb	ftz;
    time_t		Start;
    time_t		tod;

    gtdInput = p;
    if (now == NULL) {
        now = &ftz;
#if	!defined(HAVE_FTIME)
	(void)time(&ftz.time);
	/* Set the timezone global. */
	tzset();
	{
#if sgi
	    ftz.timezone = (int) _timezone / 60;
#else /* not sgi */
#ifdef __386BSD__
	    ftz.timezone = 0;
#else /* neither sgi nor 386BSD */
#if defined (USG)
	    extern time_t timezone;

	    ftz.timezone = (int) timezone / 60;
#else /* neither sgi nor 386BSD nor USG */
	    struct timeval tv;
	    struct timezone tz;
	    gettimeofday (&tv, &tz);
	    ftz.timezone = (int) tz.tz_minuteswest;
#endif /* neither sgi nor 386BSD nor USG */
#endif /* neither sgi nor 386BSD */
#endif /* not sgi */
	}
#else /* HAVE_FTIME */
	(void)ftime(&ftz);
#endif /* HAVE_FTIME */
    }

    tm = localtime(&now->time);
    gtdYear = tm->tm_year;
    gtdMonth = tm->tm_mon + 1;
    gtdDay = tm->tm_mday;
    gtdTimezone = now->timezone;
    gtdDSTmode = DSTmaybe;
    gtdHour = 0;
    gtdMinutes = 0;
    gtdSeconds = 0;
    gtdMeridian = MER24;
    gtdRelSeconds = 0;
    gtdRelMonth = 0;
    gtdHaveDate = 0;
    gtdHaveDay = 0;
    gtdHaveRel = 0;
    gtdHaveTime = 0;
    gtdHaveZone = 0;

    if (gtdparse()
     || gtdHaveTime > 1 || gtdHaveZone > 1 || gtdHaveDate > 1 || gtdHaveDay > 1)
	return -1;

    if (gtdHaveDate || gtdHaveTime || gtdHaveDay) {
	Start = Convert(gtdMonth, gtdDay, gtdYear, gtdHour, gtdMinutes, gtdSeconds,
		    gtdMeridian, gtdDSTmode);
	if (Start < 0)
	    return -1;
    }
    else {
	Start = now->time;
	if (!gtdHaveRel)
	    Start -= ((tm->tm_hour * 60L + tm->tm_min) * 60L) + tm->tm_sec;
    }

    Start += gtdRelSeconds;
    Start += RelativeMonth(Start, gtdRelMonth);

    if (gtdHaveDay && !gtdHaveDate) {
	tod = RelativeDate(Start, gtdDayOrdinal, gtdDayNumber);
	Start += tod;
    }

    /* Have to do *something* with a legitimate -1 so it's distinguishable
     * from the error return value.  (Alternately could set errno on error.) */
    return Start == -1 ? 0 : Start;
}


#if	defined(TEST)

/* ARGSUSED */
main(ac, av)
    int		ac;
    char	*av[];
{
    char	buff[128];
    time_t	d;

    (void)printf("Enter date, or blank line to exit.\n\t> ");
    (void)fflush(stdout);
    while (gets(buff) && buff[0]) {
	d = get_date(buff, (struct timeb *)NULL);
	if (d == -1)
	    (void)printf("Bad format - couldn't convert.\n");
	else
	    (void)printf("%s", ctime(&d));
	(void)printf("\t> ");
	(void)fflush(stdout);
    }
    exit(0);
    /* NOTREACHED */
}
#endif	/* defined(TEST) */
