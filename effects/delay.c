#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"

/* delay.c: a delay line */

/* parameter list: */
enum
{
	P_LEN_MS, /* outward-facing length is in ms */
	P_FEEDBACK, /* in % */
	P_WETOUTDB,
	P_DRYOUTDB
};

#define MIN_DELAY_LEN 0.01
#define MAX_DELAY_LEN 10000.0 /* (in ms) 10 sec */

typedef struct
{
	int len_in_smp;
	float wetout, dryout; /* multipliers */
	float feedback; /* multiplier */
	float *buf;
	size_t buflen; /* in sample frames */
	int bufplaypos, bufrecordpos;
} privdata_t;

static void *init(void)
{
	privdata_t *p;
	int ix;

	p = xm(sizeof *p, 1);
	p->buflen = (int)(MAX_DELAY_LEN / 1000.0 * RATE)+1;
	p->buf = xm(2*sizeof *(p->buf), p->buflen);
	p->bufplaypos = p->bufrecordpos = 0;

	for (ix = 0; ix < (int)p->buflen; ix++)
		p->buf[2*ix] = p->buf[2*ix+1] = 0.0;
	return p;
}

static void destroy(void *state)
{
	privdata_t *p = state;
	free(p->buf);
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
	p = sd->state;

	if (param == P_LEN_MS)
	{
		newval = CLAMP(MIN_DELAY_LEN, newval, MAX_DELAY_LEN);
		p->len_in_smp = newval / 1000.0 * RATE;
		p->bufrecordpos = p->bufplaypos + p->len_in_smp;
		if (p->bufrecordpos >= 2*(int)p->buflen)
			p->bufrecordpos -= 2*p->buflen;
		/* XXX: hard transition/clicks. interpolate somehow! */
	}
	else if (param == P_FEEDBACK)
		p->feedback = newval / 100.0; /* from % to multiplier */
	else if (param == P_WETOUTDB)
		p->wetout = DBTORAT(newval);
	else if (param == P_DRYOUTDB)
		p->dryout = DBTORAT(newval);
	else COMPLAIN("No such parameter");

	/* set "official" outward-facing value AFTER any clamping */
	sd->params[param].f = newval;
}

static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	int ix;
	float f[2];
	float ret[2];

	p = sd->state;

	for (ix = 0; ix < nsamps*2; ix += 2)
	{
		f[0] = p->dryout*samps[ix]
			+ p->wetout*p->buf[p->bufplaypos];
		f[1] = p->dryout*samps[ix+1]
			+ p->wetout*p->buf[p->bufplaypos+1];

		ret[0] = samps[ix] + p->feedback*p->buf[p->bufplaypos];
		ret[1] = samps[ix+1] + p->feedback*p->buf[p->bufplaypos+1];
		p->buf[p->bufrecordpos] = ret[0];
		p->buf[p->bufrecordpos+1] = ret[1];

		samps[ix] = f[0];
		samps[ix+1] = f[1];

		p->bufplaypos++, p->bufrecordpos++;
		if (p->bufplaypos >= 2*(int)p->buflen)
			p->bufplaypos -= 2*p->buflen;
		if (p->bufrecordpos >= 2*(int)p->buflen)
			p->bufrecordpos -= 2*p->buflen;
	}
}

const synthinfo_t su_delay =
{
	"delay", /* command line name */
	4, /* number of params */
	1, /* takes input */
	{
		/* P_LEN_MS */
		{
			PT_FLOAT,
			"len",
			{.f = 50.0},
			{.f = MIN_DELAY_LEN}, {.f = MAX_DELAY_LEN}, {.f = 10.0}
		},
		/* P_FEEDBACK */
		{
			PT_FLOAT,
			"feedback",
			{.f = 37.5},
			{.f = -100.0}, {.f = 100.0}, {.f = 1.0}
		},
		/* P_WETOUTDB */
		{
			PT_FLOAT,
			"wetout",
			{.f = 0.0},
			{.f = -120.0}, {.f = 20.0}, {.f = 0.1}
		},
		/* P_DRYOUTDB */
		{
			PT_FLOAT,
			"dryout",
			{.f = 0.0},
			{.f = -120.0}, {.f = 20.0}, {.f = 0.1}
		}
	},

	init, setint, setfloat, process, destroy
};
