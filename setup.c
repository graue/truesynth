#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "xm.h"
#include "synth.h"

synthdata_t *init_synth(const synthinfo_t *info)
{
	synthdata_t *unit = NULL;
	int ix;

	unit = xm(sizeof *unit, 1);
	unit->state = info->init();
	unit->info = info;

	for (ix = 0; ix < info->numparams; ix++)
	{
		if (info->paraminfo[ix].paramtype == PT_BOOL)
			info->setint(unit, ix, 0);
		else if (info->paraminfo[ix].paramtype == PT_INT)
			info->setint(unit, ix, info->paraminfo[ix].defval.n);
		else if (info->paraminfo[ix].paramtype == PT_FLOAT)
			info->setfloat(unit, ix, info->paraminfo[ix].defval.f);
		else COMPLAIN("Bad param type");
	}

	return unit;
}

/* This only takes 2 lines, but better to factor it out anyway, right? */
void destroy_synth(synthdata_t *unit)
{
	unit->info->destroy(unit->state);
	free(unit);
}
