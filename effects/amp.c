#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"

/* amp.c: adjust a waveform's volume */

/* parameter list: */
enum
{
	P_AMPDB
};

typedef struct
{
	float multiplier;
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

	COMPLAIN("amp has no bool/int params");
}

static void setfloat(synthdata_t *sd, int param, float newval)
{
	privdata_t *p;
	assert(param == P_AMPDB);
	p = sd->state;
	p->multiplier = pow(10.0, newval / 20.0);
}

static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	float mul;
	int ix;

	p = sd->state;
	mul = p->multiplier;
	for (ix = 0; ix < nsamps*2; ix += 2)
	{
		samps[ix] *= mul;
		samps[ix+1] *= mul;
	}
}

const synthinfo_t su_amp =
{
	"amp", /* command line name */
	1, /* number of params */
	1, /* takes input */
	{
		/* P_AMPDB */
		{
			PT_FLOAT,
			"dB",
			{.f = 0.0},
			{.f = -120.0}, {.f = 120.0}, {.f = 0.1}
		}
	},

	init, setint, setfloat, process, destroy
};
