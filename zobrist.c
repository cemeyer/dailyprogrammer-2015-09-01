#include <err.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "hash.h"
#include "zobrist.h"
#include "solver.h"

static FILE *frand;
static z128_t *zob_board;
static struct hash zhash;
static uint8_t *scratchb1, *scratchb2;

static void zxor(z128_t *dst, z128_t src);
static z128_t game_state(uint8_t *a_board);
static void flipboardhor(uint8_t *dst, const uint8_t *src);
static void flipboardver(uint8_t *dst, const uint8_t *src);
#if 0
static void rotateboard(uint8_t *dst, const uint8_t *src);
#endif

static z128_t
get_a_zval(void)
{
	size_t rd;
	z128_t z;

	rd = fread(&z, sizeof(z), 1, frand);
	if (rd < 1)
		err(EX_OSERR, "fread urandom");
	return (z);
}

static uint64_t
zhash_truncate(const void *v)
{
	const z128_t *z;

	z = v;
	return (z->v[0] ^ z->v[1]);
}

void
zob_init_first(void)
{

	frand = fopen("/dev/urandom", "rb");
	if (frand == NULL)
		err(EX_OSERR, "urandom");

	hash_init(&zhash, sizeof(z128_t), zhash_truncate);
}

void
zob_piece_init(struct str_to_piece *sp)
{
	struct z_pc *pc;
	unsigned i;

	pc = &sp->pc_zob;

	pc->n = sp->ct;
	pc->values = malloc(pc->n * sizeof(pc->values[0]));
	if (pc->values == NULL)
		err(EX_OSERR, "malloc");

	for (i = 0; i < sp->ct; i++)
		pc->values[i] = get_a_zval();
}

void
zob_board_init(unsigned x, unsigned y)
{
	unsigned i;

	zob_board = malloc(x * y * sizeof(*zob_board));
	if (zob_board == NULL)
		err(EX_OSERR, "malloc");

	for (i = 0; i < x * y; i++)
		zob_board[i] = get_a_zval();

	scratchb1 = malloc(board_x * board_y * sizeof(*scratchb1));
	if (scratchb1 == NULL)
		err(EX_OSERR, "malloc");
	scratchb2 = malloc(board_x * board_y * sizeof(*scratchb2));
	if (scratchb2 == NULL)
		err(EX_OSERR, "malloc");
}

bool
zob_seen_this(void)
{
	z128_t gs;

	gs = game_state(board);
	return (hash_contains(&zhash, &gs));
}

void
zob_record_this(void)
{
#if 0
	uint8_t *btmp;
#endif
	z128_t gs;

	memcpy(scratchb1, board, board_x * board_y * sizeof(*board));

#if 0
	for (i = 0; i < 4; i++) {
#endif
		gs = game_state(scratchb1);
		hash_add(&zhash, &gs);

		flipboardhor(scratchb2, scratchb1);
		gs = game_state(scratchb2);
		hash_add(&zhash, &gs);

		flipboardver(scratchb2, scratchb1);
		gs = game_state(scratchb2);
		hash_add(&zhash, &gs);

		flipboardhor(scratchb1, scratchb2);
		gs = game_state(scratchb1);
		hash_add(&zhash, &gs);

		/* Rotation only applies to uncommon square boards, skip it */
#if 0
		if (i + 1 < 4) {
			rotateboard(scratchb2, scratchb1);
			btmp = scratchb2;
			scratchb2 = scratchb1;
			scratchb1 = btmp;
		}
	}
#endif
}

static void
flipboardhor(uint8_t *dst, const uint8_t *src)
{
	unsigned x, y;

	for (x = 0; x < board_x; x++)
		for (y = 0; y < board_y; y++)
			dst[board_x * y + x] =
			    src[board_x * (board_y - 1 - y) + x];
}

static void
flipboardver(uint8_t *dst, const uint8_t *src)
{
	unsigned x, y;

	for (x = 0; x < board_x; x++)
		for (y = 0; y < board_y; y++)
			dst[board_x * y + x] =
			    src[board_x * y + (board_x - 1 - x)];
}

static z128_t
game_state(uint8_t *a_board)
{
	struct str_to_piece *sp;
	z128_t val;
	unsigned i;

	memset(&val, 0, sizeof(val));

	for (i = 0; i < board_x * board_y; i++)
		if (a_board[i] != 0)
			zxor(&val, zob_board[i]);

	for (sp = parse_lut; sp->str != NULL; sp++)
		for (i = 0; sp->pc != NULL && i < sp->ct; i++)
			zxor(&val, sp->pc_zob.values[i]);

	return (val);
}

static void
zxor(z128_t *dst, z128_t src)
{

	dst->v[0] ^= src.v[0];
	dst->v[1] ^= src.v[1];
}
