#include "synth.h"

extern const synthinfo_t su_amp;

const synthinfo_t *units[] =
{
	&su_amp
};

const int numunits = sizeof units / sizeof units[0];
