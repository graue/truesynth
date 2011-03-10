/* Help for making parameters slide. */

typedef struct
{
	float cur; /* current value */
	int sliding; /* 1=inc 0=nochange -1=dec */
	float end; /* goal value if sliding != 0 */
	float inc; /* amount to move towards goal value per sample */
	int neverbeentouched; /* on first set, set immediately */
} slide_t;

typedef struct
{
	double cur; /* current value */
	int sliding; /* 1=inc 0=nochange -1=dec */
	double end; /* goal value if sliding != 0 */
	double inc; /* amount to move towards goal value per sample */
	int neverbeentouched; /* on first set, set immediately */
} dblslide_t;

#define DOSLIDE(s) do { \
	if (s.sliding) { \
		s.cur += s.inc; \
		if (s.sliding > 0 && s.cur >= s.end) { \
			s.cur = s.end; \
			s.sliding = 0; \
		} else if (s.cur <= s.end) { \
			s.cur = s.end; \
			s.sliding = 0; \
		} \
	} \
} while (0)

/* Set a slide value immediately and make it not move - for initialization. */
#define SETSLIDE(s,f) do { \
	s.cur = s.end = (f); \
	s.sliding = 0; \
	s.inc = 0.0; \
} while (0)

/* s = slide value, f = goal, t = units of time before making it there. */
#define SETSLIDEGOAL(s,f,t) do { \
	if (s.neverbeentouched) { \
		s.neverbeentouched = 0; \
		SETSLIDE(s,(f)); \
	} else { \
		s.end = (f); \
		s.inc = (s.end - s.cur) / (t); \
		if (s.inc > 0.0) \
			s.sliding = 1; \
		else if (s.inc < 0.0) \
			s.sliding = -1; \
		else s.sliding = 0; \
	} \
} while (0)

#define INITSLIDE(s) s.neverbeentouched = 1
