#include <string.h>
#include "synth.h"

extern const synthinfo_t
	su_amp,
	su_clip,
	su_pan;

const synthinfo_t *units[] =
{
	&su_amp,
	&su_clip,
	&su_pan
};

const int numunits = sizeof units / sizeof units[0];

const synthinfo_t *find_unit(const char *name)
{
	int ix;
	for (ix = 0; ix < numunits; ix++)
		if (!strcmp(name, units[ix]->cmdname))
			return units[ix];
	return NULL;
}
