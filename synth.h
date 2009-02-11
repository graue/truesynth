/* Definitions for the audio used: */

#ifndef DEFAULT_RATE
#define DEFAULT_RATE 44100
#endif

extern int RATE; /* always set this by calling get_rate() */
void get_rate(void);

#ifndef LACK_UNISTD_H
#include <unistd.h>
#endif

/* Useful stuff: */

/*
 * SET_BINARY_MODE(x) sets the given file descriptor to binary
 * if required (only on Win32), and also checks to make sure
 * it's not a terminal, if that is supported (Unix-like OSes).
 * 0 for stdin, 1 for stdout.
 */

#ifdef _WIN32
# include <io.h>
# include <fcntl.h>
# define ACTUALLY_SET_BINARY_MODE(x) setmode(x, O_BINARY);
#else
# define ACTUALLY_SET_BINARY_MODE(x) ((void)0);
#endif

#ifdef LACK_UNISTD_H
#define SET_BINARY_MODE(x) ACTUALLY_SET_BINARY_MODE(x)
#else
#define SET_BINARY_MODE(x) { \
	ACTUALLY_SET_BINARY_MODE(x) \
	if (isatty(x)) { \
		fprintf(stderr, "std%s should not be a tty\n", \
			x == 0 ? "in" : "out"); \
		exit(EXIT_FAILURE); \
	} \
}
#endif

#define CLAMP(min, n, max) ((n)<(min)?(min):((n)>(max)?(max):(n)))

/*
 * more informative substitute for assert(0) in code that
 * should never be reached.
 * example usage: COMPLAIN("No float parameter exists for so-and-so effect");
 */
#define COMPLAIN(s) assert(((void)s, 0))

/* Unit definitions: */

#define MAX_UNIT_PARAMS 50

/* parameter value types */
#define PT_INT 0
#define PT_FLOAT 1
#define PT_BOOL 2 /* always defaults to off */

typedef union
{
	int n;
	float f;
} paramval_t;

typedef struct
{
	int paramtype; /* PT_... */
	const char *name; /* e.g. "dB" */
	paramval_t defval;
	paramval_t min, max, increment;
} synthparaminfo_t;

struct synthinfo_s;

/* this one is specific to an instance of a unit */
typedef struct
{
	const struct synthinfo_s *info; /* ptr to unit type info */
	paramval_t params[MAX_UNIT_PARAMS];
	void *state; /* whatever other state the unit needs */
} synthdata_t;

typedef struct synthinfo_s
{
	char *cmdname; /* command line name */
	int numparams; /* number of parameters */
	int acceptsinput; /* boolean: 1 = is an effect, 0 = generator */
	synthparaminfo_t paraminfo[MAX_UNIT_PARAMS];

	/*
	 * Function pointers for the synth unit:
	 *
	 * When a unit instance is created, init() is
	 * called to allocate, initialize, and return a
	 * pointer to the internal state. Then set() is
	 * called once for each parameter to set it to
	 * its default value. process() is then used to
	 * process a block of samples. Eventually
	 * destroy() is called to free the unit's
	 * internal state.
	 */

	void *(*init)(void); /* initialization function */
	void (*setint)(synthdata_t *, int, int); /* set bool/int param */
	void (*setfloat)(synthdata_t *, int, float); /* set float param */
	void (*process)(synthdata_t *, int, float *); /* process samples */
	void (*destroy)(void *); /* destructor; free the state etc. */
} synthinfo_t;
