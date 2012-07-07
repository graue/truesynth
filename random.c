#include <stdint.h>

/* Adapted from Bob Jenkins's "A small noncryptographic PRNG".
 *
 * http://www.burtleburtle.net/bob/rand/smallprng.html
 *
 * As accessed on July 7, 2012, the above web page (which contains the
 * code ts_urand() and ts_randinit() are based on) states:
 *   "I wrote this PRNG. I place it in the public domain."
 */

typedef struct ranctx {
	uint32_t a, b, c, d;
} ranctx;

static ranctx ctx;

#define rot(x,k) (((x)<<(k))|((x)>>(32-(k))))
uint32_t ts_urand(void)
{
	uint32_t e = ctx.a - rot(ctx.b, 27);
	ctx.a = ctx.b ^ rot(ctx.c, 17);
	ctx.b = ctx.c + ctx.d;
	ctx.c = ctx.d + e;
	ctx.d = e + ctx.a;
	return ctx.d;
}

void ts_randinit(uint32_t seed)
{
	uint32_t i;
	ctx.a = 0xf1ea5eed;
	ctx.b = ctx.c = ctx.d = seed;
	for (i = 0; i < 20; i++)
		(void)ts_urand();
}

int32_t ts_rand(void) {
	return (int32_t)(ts_urand() / 2);
}

double ts_frand(void) {
	double d = ts_urand();
	d /= (0xffffffffU + 1.0);
	return d;
}
