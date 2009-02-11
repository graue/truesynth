#ifndef SYNTH_NO_SHELL

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include "err.h"
#include "xm.h"
#include "mt.h"
#include "synth.h"
#include "setup.h"
#include "info.h"

/* Default no. of sample frames to process at a time. */
#define DEFAULT_BUFSMPS 1000

static void listunits(void)
{
	int ix;

	for (ix = 0; ix < numunits; ix++)
		printf("%s\n", units[ix]->cmdname);
}

static void listinfo(const char *cmdname, const synthinfo_t *info)
{
	int ix;
	int paramtype;
	const char *paramname;
	paramval_t defval, min, max;

	printf("%s is %s:\n", cmdname,
		info->acceptsinput ? "an effect" : "a generator");

	for (ix = 0; ix < info->numparams; ix++)
	{
		paramtype = info->paraminfo[ix].paramtype;
		paramname = info->paraminfo[ix].name;
		defval = info->paraminfo[ix].defval;
		min = info->paraminfo[ix].min;
		max = info->paraminfo[ix].max;

		putchar(' ');
		if (paramtype == PT_BOOL)
			printf("-%s on|off [off]\n", paramname);
		else if (paramtype == PT_INT)
			printf("-%s %d..%d [%d]\n", paramname,
				min.n, max.n, defval.n);
		else if (paramtype == PT_FLOAT)
			printf("-%s %.2f..%.2f [%.2f]\n", paramname,
				min.f, max.f, defval.f);
		else COMPLAIN("Bad param type");
	}
}

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
	mt_init((unsigned int)time(NULL));

	if (argc < 2)
	{
		fprintf(stderr, "usage: synth unitname [-param val] ...\n");
		exit(1);
	}

	cmdname = argv[1];
	if (!strcmp(cmdname, "help") || !strcmp(cmdname, "-help"))
	{
		listunits();
		exit(0);
	}
	unittype = find_unit(cmdname);

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
		if (!strcmp(paramname, "buffer"))
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

		if (!strcmp(paramname, "help"))
		{
			listinfo(cmdname, unittype);
			exit(0);
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
			warnx("%s: ignored unknown param \"-%s\"",
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

#endif
