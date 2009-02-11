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
