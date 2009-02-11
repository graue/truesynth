#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"

/* pan.c: adjust a waveform's horizontal panning */

/*
 * GOTCHA WARNING: angle is OPPOSITE that of classic synth.
 * As angle increases you pan towards the RIGHT.
 * This makes more sense (the opposite behavior in classic pan
 * never made any sense).
 */

/* parameter list: */
enum
{
	P_ANGLEDEG
};

typedef struct
{
	float leftmul, rightmul;
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
	float anglerad;

	assert(param == P_ANGLEDEG);

	anglerad = newval * M_PI / 180.0;

	p = sd->state;
	p->leftmul = cos(anglerad) - sin(anglerad);
	p->rightmul = cos(anglerad) + sin(anglerad);
}

static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	int ix;

	p = sd->state;
	for (ix = 0; ix < nsamps*2; ix += 2)
	{
		/* XXX optimize - remove dereference? or pointless? */
		samps[ix] *= p->leftmul;
		samps[ix+1] *= p->rightmul;
	}
}

const synthinfo_t su_pan =
{
	"pan", /* command line name */
	1, /* number of params */
	1, /* takes input */
	{
		/* P_ANGLEDEG */
		{
			PT_FLOAT,
			"angle",
			{.f = 0.0},
			{.f = -180.0}, {.f = 180.0}, {.f = 1.0}
		}
	},

	init, setint, setfloat, process, destroy
};
