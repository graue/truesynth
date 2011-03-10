#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "../synth.h"
#include "../xm.h"
#include "../slide.h"

/* osc.c: sine/square/triangle/saw oscillator */

/*
Note: This uses doubles for phase and phase increment.
The old synth code used doubles in 'sine', but floats for the other
oscillators. So those other oscillators will get slightly out of sync
with time, relative to the old synth code.
*/

/* oscillator types (private) */
#define OSC_SINE 0
#define OSC_SQUARE 1
#define OSC_TRI 2
#define OSC_SAWUP 3
#define OSC_SAWDOWN 4
#define OSC_NUMTYPES 5

#define AMP_SLIDE_MS 1
#define FREQ_SLIDE_MS 1

#define DEFAMP -12
#define DEFFREQ 1000

/* parameter list: */
enum
{
	P_OSCTYPE,
	P_AMPDB,
	P_FREQ
};

typedef struct
{
	int osctype;
	slide_t multiplier;
	double phase;
	dblslide_t phaseinc; /* (2*M_PI * freq / RATE) */

	int started; /* processing started? if 0, don't slide params yet */
} privdata_t;

static void *init(void)
{
	privdata_t *p;
	p = xm(sizeof *p, 1);
	p->osctype = OSC_SINE;
	//SETSLIDE(p->multiplier, DBTORAT(DEFAMP));
	p->phase = 0.0;
	//SETSLIDE(p->phaseinc, 2*M_PI * DEFFREQ / RATE);
	INITSLIDE(p->multiplier);
	INITSLIDE(p->phaseinc);
	p->started = 0;
	return p;
}

static void destroy(void *state)
{
	free(state);
}

static void setint(synthdata_t *sd, int param, int newval)
{
	privdata_t *p;

	if (param == P_OSCTYPE)
	{
		if (newval >= 1 && newval < OSC_NUMTYPES)
		{
			sd->params[param].n = newval;
			p = sd->state;
			p->osctype = newval;
		}
	}
	else
		COMPLAIN("osc: Invalid int parameter");
}

static void setfloat(synthdata_t *sd, int param, float newval)
{
	privdata_t *p = sd->state;

	if (param == P_FREQ)
	{
		if (newval < 0.1)
			newval = 0.1;
		else if (newval > RATE/2.0)
			newval = RATE/2.0;

		sd->params[param].f = newval;
		if (p->started)
		{
			SETSLIDEGOAL(p->phaseinc, 2*M_PI * newval / RATE,
				FREQ_SLIDE_MS * RATE / 1000.0);
		}
		else
			SETSLIDE(p->phaseinc, 2*M_PI * newval / RATE);
	}
	else if (param == P_AMPDB)
	{
		sd->params[param].f = newval;
		if (p->started)
		{
			SETSLIDEGOAL(p->multiplier, DBTORAT(newval),
				AMP_SLIDE_MS * RATE / 1000.0);
		}
		else
			SETSLIDE(p->multiplier, DBTORAT(newval));
	}
}

#define mul p->multiplier
#define phase p->phase
static void process(synthdata_t *sd, int nsamps, float *samps)
{
	privdata_t *p;
	float f;
	int ix;

	p = sd->state;

	for (ix = 0; ix < nsamps*2; ix += 2)
	{
		DOSLIDE(mul);
		DOSLIDE(p->phaseinc);
		if (p->osctype == OSC_SINE)
			samps[ix] = mul.cur * sin(phase);
		else if (p->osctype == OSC_SQUARE)
		{
			if (fmod(phase, 2*M_PI) < M_PI)
				samps[ix] = mul.cur;
			else
				samps[ix] = -mul.cur;
		}
		else if (p->osctype == OSC_TRI)
		{
			f = fmod(phase, 2*M_PI);
			if (f < 0.5*M_PI)
				samps[ix] = mul.cur * (f/(0.5*M_PI));
			else if (f < 1.5*M_PI)
				samps[ix] = mul.cur * (-f/(0.5*M_PI) + 2.0);
			else
				samps[ix] = mul.cur * (f/(0.5*M_PI) - 4.0);
		}
		else if (p->osctype == OSC_SAWUP)
		{
			f = fmod(phase + M_PI, 2*M_PI);
			samps[ix] = mul.cur * (f/M_PI - 1.0);
		}
		else if (p->osctype == OSC_SAWDOWN)
		{
			f = fmod(phase + M_PI, 2*M_PI);
			samps[ix] = -mul.cur * (f/M_PI - 1.0);
		}
		samps[ix+1] = samps[ix];
		phase += p->phaseinc.cur;
	}
	p->started = 1;
}

const synthinfo_t su_osc =
{
	"osc", /* command line name */
	3, /* number of params */
	0, /* takes input */
	{
		/* P_OSCTYPE */
		{
			PT_INT,
			"osctype",
			{.n = 0}, /* default */
			{.n = 0}, {.n = 4}, {.n = 1} /* min/max/step */
		},

		/* P_AMPDB */
		{
			PT_FLOAT,
			"amp",
			{.f = DEFAMP},
			{.f = -120.0}, {.f = 120.0}, {.f = 0.1}
		},

		/* P_FREQ */
		{
			PT_FLOAT,
			"freq",
			{.f = DEFFREQ},
			{.f = 0.1}, {.f = 24000.0}, {.f = 1.0}
		}
	},

	init, setint, setfloat, process, destroy
};
