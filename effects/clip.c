#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"

/* clip.c: clip a waveform */

/* parameter list: */
enum
{
	P_THRESHDB
};

typedef struct
{
	float max;
} privdata_t;

static void *init(void)
{
	privdata_t *p;
	p = xm(sizeof *p, 1);
	return p;
}

static void destroy(void *state)
{
	free(state);
}

static void setint(synthdata_t *sd, int param, int newval)
{
	(void)sd, (void)param, (void)newval; /* not used */

	COMPLAIN("This machine has no bool/int params");
}

static void setfloat(synthdata_t *sd, int param, float newval)
{
	privdata_t *p;
	assert(param == P_THRESHDB);
	sd->params[param].f = newval;
	p = sd->state;
	p->max = pow(10.0, newval / 20.0);
}

static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	int ix;

	p = sd->state;
	for (ix = 0; ix < nsamps*2; ix += 2)
	{
		samps[ix] = CLAMP(-p->max, samps[ix], p->max);
		samps[ix+1] = CLAMP(-p->max, samps[ix+1], p->max);
	}
}

const synthinfo_t su_clip =
{
	"clip", /* command line name */
	1, /* number of params */
	1, /* takes input */
	{
		/* P_THRESHDB */
		{
			PT_FLOAT,
			"dB",
			{.f = 0.0},
			{.f = -120.0}, {.f = 120.0}, {.f = 0.1}
		}
	},

	init, setint, setfloat, process, destroy
};
