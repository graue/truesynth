#include <stdlib.h>
#include "synth.h"

/*
 * This defaults to -1, not DEFAULT_RATE, in case any
 * code forgets to call get_rate(). It should fail
 * badly, rather than work silently until a non-default
 * sample rate is used. ALWAYS call get_rate().
 */
int RATE = -1;

void get_rate(void)
{
	char *str;

	str = getenv("RATE");
	if (str == NULL)
		str = getenv("SR");

	if (str == NULL || atoi(str) <= 0)
		RATE = DEFAULT_RATE;
	else
		RATE = atoi(str);
}
