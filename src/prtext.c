#include <stdio.h>

#include "global.h"

#include "print.h"

void TextJobHeader(char *str, int npages, FILE *fp)
{
}

void TextJobTrailer(FILE *fp)
{
}

void TextPageHeader(char *str, FILE *fp)
{
}

void TextPageFooter(char *str, FILE *fp)
{
}

void TextField(char *str, int wid, int rightborder, FILE *fp)
{
}

void TextBorders(FILE *fp)
{
}

void TextFont(char *fn, FILE *fp)
{
}

void TextNewLine(int ht, FILE *fp)
{
}

struct PrintDriver TextPrintDriver = {
	"Text",
	&TextJobHeader,
	&TextJobTrailer,
	&TextPageHeader,
	&TextPageFooter,
	&TextField,
	&TextBorders,
	&TextFont,
	&TextNewLine
};
