typedef union {
    int			Number;
    enum _MERIDIAN	Meridian;
} YYSTYPE;
#define	tAGO	258
#define	tDAY	259
#define	tDAY_UNIT	260
#define	tDAYZONE	261
#define	tDST	262
#define	tHOUR_UNIT	263
#define	tID	264
#define	tMERIDIAN	265
#define	tMINUTE_UNIT	266
#define	tMONTH	267
#define	tMONTH_UNIT	268
#define	tSEC_UNIT	269
#define	tSNUMBER	270
#define	tUNUMBER	271
#define	tYEAR_UNIT	272
#define	tZONE	273


extern YYSTYPE yylval;
