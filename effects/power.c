#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"

/* power.c: raise each of a waveform's samples to a power */

/* parameter list: */
enum
{
	P_EXP
};

typedef struct
{
	float exponent;
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
	assert(param == P_EXP);
	sd->params[param].f = newval;
	p = sd->state;
	p->exponent = newval;
}

static float powerify(float f, int positiveexponent, int wholeexponent,
	int signedexp, float exponent)
{
	if (!signedexp)
	{
		if (positiveexponent || f != 0.0f)
		{
			if (f > 0.0f)
				f = pow(f, exponent);
			else
				f = -pow(-f, exponent);
		}
	}
	else if (!(!wholeexponent && f < 0.0f)
		&& !(!positiveexponent && f == 0.0f))
	{
		f = pow(f, exponent);
	}

	return f;
}

static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	float exponent;
	int positiveexponent;
	int wholeexponent;
	int signedexp = 0; /* XXX not implemented yet as an option, so always 0 for now */
	int ix;

	p = sd->state;
	exponent = p->exponent;
	positiveexponent = exponent > 0.0f;
	wholeexponent = exponent == floorf(exponent);
	for (ix = 0; ix < nsamps*2; ix++)
	{
		samps[ix] = powerify(samps[ix], positiveexponent,
			wholeexponent, signedexp, exponent);
	}
}

const synthinfo_t su_power =
{
	"power", /* command line name */
	1, /* number of params */
	1, /* takes input */
	{
		/* P_EXP */
		{
			PT_FLOAT,
			"exp",
			{.f = 1.0},
			{.f = 0.0}, {.f = 10.0}, {.f = 0.1}
		}
	},

	init, setint, setfloat, process, destroy
};
