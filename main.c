#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "err.h"
#include "xm.h"
#include "synth.h"
#include "setup.h"
#include "info.h"

/* Default no. of sample frames to process at a time. */
#define DEFAULT_BUFSMPS 1000

int main(int argc, char *argv[])
{
	int ix, jx;
	int readcnt, writecnt;
	int bufsmps = DEFAULT_BUFSMPS;
	const synthinfo_t *unittype = NULL;
	const char *cmdname;
	synthdata_t *unit = NULL;
	float *samps;

	get_rate();

	if (argc < 2)
	{
		fprintf(stderr, "usage: synth unitname [-param val] ...\n");
		exit(1);
	}

	cmdname = argv[1];
	for (ix = 0; ix < numunits; ix++)
	{
		if (!strcmp(cmdname, units[ix].cmdname))
		{
			unittype = &units[ix];
			break;
		}
	}

	if (unittype == NULL)
		errx(1, "no such unit \"%s\"", cmdname);

	unit = init_synth(unittype);

	/* scan for parameters in command line */
	for (ix = 2; ix < argc; ix++)
	{
		const char *paramname;
		int pindex = -1; /* parameter index */
		const synthparaminfo_t *param = NULL;

		if (argv[ix][0] != '-')
		{
			warnx("%s: ignored non-option parameter \"%s\"",
				cmdname, argv[ix]);
			continue;
		}

		paramname = &argv[ix][1];

		/* special block size/buffering option for all effects */
		if (!strcmp(paramname, "-buffer"))
		{
			if (ix+1 >= argc) /* not enough args */
			{
				errx(1, "%s: \"-buffer\" needs an argument",
					cmdname);
			}
			ix++;
			bufsmps = atoi(argv[ix]);
			if (bufsmps <= 0)
			{
				errx(1, "%s: buffer size must be positive",
					cmdname);
			}
			continue;
		}

		for (jx = 0; jx < unittype->numparams; jx++)
		{
			if (!strcmp(paramname, unittype->paraminfo[jx].name))
			{
				pindex = jx;
				param = &unittype->paraminfo[pindex];
				break;
			}
		}

		if (param == NULL)
		{
			warnx("%s: param \"-%s\" not found",
				cmdname, paramname);

			/*
			 * if skipped param has a setting skip it,
			 * e.g. "-thisoptiondoesntexist 2"
			 */
			if (ix+1 < argc && argv[ix+1][0] != '-')
				ix++;

			continue;
		}

		if (param->paramtype == PT_BOOL)
			unittype->setint(unit, pindex, 1);
		else if (ix+1 >= argc) /* not enough args */
		{
			errx(1, "%s: \"-%s\" needs an argument",
				cmdname, paramname);
		}
		else if (param->paramtype == PT_INT)
		{
			ix++;
			unittype->setint(unit, pindex, atoi(argv[ix]));
		}
		else if (param->paramtype == PT_FLOAT)
		{
			ix++;
			unittype->setfloat(unit, pindex, atof(argv[ix]));
		}
		else COMPLAIN("Bad param type");
	}

	SET_BINARY_MODE(1) /* stdout */
	if (unittype->acceptsinput)
	{
		SET_BINARY_MODE(0) /* stdin */
	}

	samps = xm(sizeof *samps, bufsmps*2);

	while ((readcnt = fread(samps, 2*sizeof samps[0], bufsmps, stdin)) > 0)
	{
		unittype->process(unit, readcnt, samps);

		writecnt = fwrite(samps, 2*sizeof samps[0], readcnt, stdout);

		/* If we can't write anymore, just quit - pipe prob. broken. */
		if (writecnt < readcnt)
			break;
	}

	return 0;
}
