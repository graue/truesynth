#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"

/* shape.c: waveshape a waveform */

/* parameter list: */
enum
{
	P_RANGE,
	P_GRADATION
};

typedef struct
{
	float range;
	float gradation;
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
	sd->params[param].f = newval;
	p = sd->state;
	if (param == P_RANGE)
		p->range = newval;
	else if (param == P_GRADATION)
		p->gradation = newval;
	else
		COMPLAIN("Shape param is out of range");
}

static float shapeval(float val, float r, float g)
{
	float x = val;

	if (x < -r/(g+1))
		x = x/g - r/(g+1) * (g-1/g);
	else if (x > r/(g+1))
		x = x/g + r/(g+1) * (g-1/g);
	else
		x *= g;
	return x;
}

static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	int ix;
	float r, g;

	p = sd->state;
	r = p->range;
	g = p->gradation;
	for (ix = 0; ix < nsamps*2; ix++)
		samps[ix] = shapeval(samps[ix], r, g);
}

const synthinfo_t su_shape =
{
	"shape", /* command line name */
	2, /* number of params */
	1, /* takes input */
	{
		/* P_RANGE */
		{
			PT_FLOAT,
			"range",
			{.f = 1.0},
			{.f = -2.0}, {.f = 2.0}, {.f = 0.05}
		},
		/* P_GRADATION */
		{
			PT_FLOAT,
			"gradation",
			{.f = 3.0},
			{.f = 0.1}, {.f = 15.0}, {.f = 0.1}
		}
	},

	init, setint, setfloat, process, destroy
};
