#include <stdio.h>

#include "global.h"

#include "print.h"

void TextInitialize(FILE *fp)
{
}

struct PrintDriver TextPrintDriver = {
	"Text",
	&TextInitialize,
};
