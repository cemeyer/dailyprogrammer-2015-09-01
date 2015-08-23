#include <err.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#define	ARRAY_LEN(a)	(sizeof(a) / sizeof(a[0]))
#define	MIN(a, b)	({						\
	typeof(a) _min1 = (a);						\
	typeof(b) _min2 = (b);						\
	(void) (&_min1 == &_min2);					\
	_min1 < _min2 ? _min1 : _min2; })

struct piece {
	unsigned n_orient;
	struct piece_or {
		unsigned x, y;
		int bitmap[6];
	} orients[4];
};

#define	X	1
#define	_	0

/*
 * ##
 *  ##
 */
static const struct piece egypt1 = {
	.n_orient = 2,
	.orients = {
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				X, X, _,
				_, X, X,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				_, X,
				X, X,
				X, _,
			},
		},
	},
};

/*
 *  ##
 * ##
 */
static const struct piece egypt2 = {
	.n_orient = 2,
	.orients = {
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				_, X, X,
				X, X, _,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				X, _,
				X, X,
				_, X,
			},
		},
	},
};

/*
 * ####
 */
static const struct piece line = {
	.n_orient = 2,
	.orients = {
		{
			.x = 4,
			.y = 1,
			.bitmap = {
				X, X, X, X,
			},
		},
		{
			.x = 1,
			.y = 4,
			.bitmap = {
				X,
				X,
				X,
				X,
			},
		},
	},
};

/*
 * ###
 * #
 */
static const struct piece ell1 = {
	.n_orient = 4,
	.orients = {
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				X, X, X,
				X, _, _,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				X, X,
				_, X,
				_, X,
			},
		},
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				_, _, X,
				X, X, X,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				X, _,
				X, _,
				X, X,
			},
		},
	},
};

/*
 * ###
 *   #
 */
static const struct piece ell2 = {
	.n_orient = 4,
	.orients = {
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				X, X, X,
				_, _, X,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				_, X,
				_, X,
				X, X,
			},
		},
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				X, _, _,
				X, X, X,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				X, X,
				X, _,
				X, _,
			},
		},
	},
};

/*
 *  #
 * ###
 */
static const struct piece triangle = {
	.n_orient = 4,
	.orients = {
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				X, X, X,
				_, X, _,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				_, X,
				X, X,
				_, X,
			},
		},
		{
			.x = 3,
			.y = 2,
			.bitmap = {
				_, X, _,
				X, X, X,
			},
		},
		{
			.x = 2,
			.y = 3,
			.bitmap = {
				X, _,
				X, X,
				X, _,
			},
		},
	},
};

/*
 * ##
 * ##
 */
static const struct piece square = {
	.n_orient = 1,
	.orients = {
		{
			.x = 2,
			.y = 2,
			.bitmap = {
				X, X,
				X, X,
			},
		},
	},
};
#undef	X
#undef	_

struct str_to_piece {
	const char *str;
	const char *std_name;
	const struct piece *pc;

	unsigned ct;
};

static unsigned total_pc;
static struct str_to_piece parse_lut[] = {
#define	X_IDX	0
	{ "x",  "x", NULL },
#define	Y_IDX	1
	{ "y",  "y", NULL },

	{ "e1", "Z", &egypt1 },
	{ "e2", "S", &egypt2 },
	{ "ln", "I", &line },
	{ "l1", "L", &ell1 },
	{ "l2", "J", &ell2 },
	{ "tr", "T", &triangle },
	{ "sq", "O", &square },
	{ 0 }
};

static void
parse_set(const char *what, unsigned howmany, unsigned line)
{
	struct str_to_piece *sp;

	for (sp = parse_lut; sp->str != NULL; sp++) {
		if (strcmp(sp->str, what) != 0 &&
		    strcmp(sp->std_name, what) != 0)
			continue;

		if (sp->ct != 0)
			errx(EX_USAGE, "one line per type (line: %u, type: %s)",
			    line, what);

#if 0
		printf("XXX: %s: [%u] %s = %u\n", __func__, line, what,
		    howmany);
#endif
		sp->ct = howmany;
		if (sp->pc != NULL)
			total_pc += howmany;
		return;
	}

	errx(EX_USAGE, "unrecognized type %s at line: %u", what, line);
}

static void
parse(FILE *f)
{
	char str[8];
	unsigned ct, line, x, y;
	int n;

	memset(str, 0, sizeof(str));

	for (line = 1;; line++) {
		n = fscanf(f, "%6s = %u\n", str, &ct);
		if (n == EOF)
			break;

		if (n < 2)
			errx(EX_USAGE, "failed to parse at line: %u", line);

		parse_set(str, ct, line);
	}

	x = parse_lut[X_IDX].ct;
	y = parse_lut[Y_IDX].ct;
	if (x == 0 || y == 0 || (x * y) != (4 * total_pc))
		errx(EX_USAGE, "%s: mismatched board: [%u x %u] and %u pieces",
		    __func__, x, y, total_pc);
}

struct move {
	const char *p_name;
	unsigned p_orient, x, y;
};

static struct move move[128];
static unsigned board_x, board_y;
static uint64_t board_tries;
static uint8_t *board;
static uint8_t *board_flood;

#define	BOARD_SQ(x, y)	board[ board_x * (y) + (x) ]
#define	FILL_SQ(x, y)	board_flood[ board_x * (y) + (x) ]

enum plhow {
	PLACE_TRY,
	PLACE_DO,
	PLACE_UNDO,
};

static bool
_tryplace(unsigned x, unsigned y, const struct piece_or *orp, unsigned *ct,
    enum plhow how)
{
	unsigned spr_x, spr_y;
	unsigned pix_x, pix_y;

	if (how == PLACE_UNDO) {
		total_pc++;
		(*ct)++;
	}

	for (spr_x = 0; spr_x < orp->x; spr_x++) {
		for (spr_y = 0; spr_y < orp->y; spr_y++) {
			if (orp->bitmap[orp->x * spr_y + spr_x] == 0)
				continue;

			pix_x = x + spr_x;
			pix_y = y + spr_y;
			if (board[board_x * pix_y + pix_x]) {
				// already set
				if (how == PLACE_TRY)
					return (false);
				else if (how == PLACE_DO)
					errx(EX_OSERR, "i dumb %d", __LINE__);
				else if (how == PLACE_UNDO)
					board[board_x * pix_y + pix_x] = 0;
			} else {
				// empty
				if (how == PLACE_DO)
					board[board_x * pix_y + pix_x] = 1;
				else if (how == PLACE_UNDO)
					errx(EX_OSERR, "i dumb %d", __LINE__);
			}
		}
	}

	if (how == PLACE_DO) {
#if 0
		struct str_to_piece *sp;
		for (sp = parse_lut; sp->str != NULL; sp++)
			if (&sp->ct == ct)
				printf("XXX place %s at %u,%u orient %lu\n",
				    sp->str, x, y, orp - sp->pc->orients);
#endif

		total_pc--;
		(*ct)--;
	}

	return (true);

}
#define	canplace(x, y, o)	_tryplace(x, y, o, NULL, PLACE_TRY)
#define	place(x, y, o, p)	_tryplace(x, y, o, p, PLACE_DO)
#define	unplace(x, y, o, p)	_tryplace(x, y, o, p, PLACE_UNDO)

static void
printboard(unsigned depth)
{
	unsigned x, y;

#if 0
	static unsigned print_count;
	print_count++;
	if (print_count > 10)
		errx(0, "%s: done spamming", __func__);
#endif

	printf("\nBoard @ depth=%u (piece remain=%u)\n", depth, total_pc);
	for (y = 0; y < board_y; y++) {
		for (x = 0; x < board_x; x++) {
			if (board[board_x * y + x] != 0)
				putchar('#');
			else
				putchar(' ');
		}
		puts("|");
	}
	for (x = 0; x < board_x; x++)
		putchar('-');
	puts("");
}

static void
printpiece(const char *nm, unsigned orient)
{
	const struct piece_or *orp;
	struct str_to_piece *sp;
	unsigned spr_x, spr_y;

	for (sp = parse_lut; sp->str != NULL; sp++)
		if (strcmp(sp->std_name, nm) == 0)
			break;

	if (sp->str == NULL)
		errx(EX_OSERR, "i dumb %d", __LINE__);
	if (sp->pc == NULL)
		errx(EX_OSERR, "i dumb %d", __LINE__);

	orp = &sp->pc->orients[orient];
	for (spr_y = 0; spr_y < orp->y; spr_y++) {
		putchar('\t');
		for (spr_x = 0; spr_x < orp->x; spr_x++) {
			if (orp->bitmap[orp->x * spr_y + spr_x] == 0)
				putchar(' ');
			else
				putchar('#');
		}
		putchar('\n');
	}
}

static void
win(unsigned depth)
{
	unsigned i;

	printf("Win!\n\n");
	for (i = 0; i < depth; i++) {
		printf("Piece %s at [%u,%u] orientation %u\n", move[i].p_name,
		    move[i].x, move[i].y, move[i].p_orient);
		printpiece(move[i].p_name, move[i].p_orient);
	}
	fflush(stdout);
	exit(0);
}

/* It saves work to only clean once at the end */
static unsigned
floodfillct(unsigned x, unsigned y)
{
	unsigned ct;
	int dx, dy;

	if (x >= board_x || y >= board_y)
		return (0);

	if (BOARD_SQ(x, y) != 0)
		return (0);

	if (FILL_SQ(x, y) != 0)
		return (0);

	FILL_SQ(x, y) = 1;
	ct = 1;

	for (dx = -1; dx < 2; dx += 2)
		ct += floodfillct(x + dx, y);
	for (dy = -1; dy < 2; dy += 2)
		ct += floodfillct(x, y + dy);

	return (ct);
}

/*
 * Flood fill from piece edges; if any space is not mod 4 blocks, the board is
 * toast; return TRUE.
 */
static bool
holemodfail(unsigned x, unsigned y, const struct piece_or *orp)
{
	unsigned ct, spr_x, spr_y, px, py;
	bool fail;

	fail = false;

	/* Check possible boundary pixels around the newly placed piece. */
	for (spr_x = 0; spr_x < orp->x; spr_x++) {
		for (spr_y = 0; spr_y < orp->y; spr_y++) {
			if (orp->bitmap[orp->x * spr_y + spr_x] != 0)
				continue;

			ct = floodfillct(x + spr_x, y + spr_y);
			if (ct % 4 != 0) {
				fail = true;
				goto out;
			}
		}
	}

	/* Four borders */
	for (px = MIN(x - 1, x); px < x + orp->x + 1; px++) {
		/* top */
		if (y - 1 < board_y) {
			ct = floodfillct(px, y - 1);
			if (ct % 4 != 0) {
				fail = true;
				goto out;
			}
		}
		/* bottom */
		if (y + orp->y < board_y) {
			ct = floodfillct(px, y + orp->y);
			if (ct % 4 != 0) {
				fail = true;
				goto out;
			}
		}
	}
	for (py = MIN(y - 1, y); py < y + orp->y + 1; py++) {
		/* left */
		if (x - 1 < board_x) {
			ct = floodfillct(x - 1, py);
			if (ct % 4 != 0) {
				fail = true;
				goto out;
			}
		}
		/* right */
		if (x + orp->x < board_x) {
			ct = floodfillct(x + orp->x, py);
			if (ct % 4 != 0) {
				fail = true;
				goto out;
			}
		}
	}

out:
	memset(board_flood, 0, board_x * board_y * sizeof(board_flood[0]));
	return (fail);
}

static void
solve(unsigned depth)
{
	struct str_to_piece *sp;

	const struct piece_or *orp;
	unsigned or;
	unsigned or_x, or_y;
	unsigned p_x, p_y;

	if (depth >= ARRAY_LEN(move))
		errx(EX_OSERR, "i dumb");

	/* If we're out of pieces, print successful placements and exit */
	if (total_pc == 0)
		win(depth);

	/* If we can, play a piece */
	for (sp = parse_lut; sp->str != NULL; sp++) {
		if (sp->pc == NULL)
			continue;

		if (sp->ct == 0)
			continue;

		for (or = 0; or < sp->pc->n_orient; or++) {
			orp = &sp->pc->orients[or];
			or_x = orp->x;
			or_y = orp->y;

			for (p_x = 0; p_x + or_x <= board_x; p_x++) {
				for (p_y = 0; p_y + or_y <= board_y; p_y++) {
					if (!canplace(p_x, p_y, orp))
						continue;

					place(p_x, p_y, orp, &sp->ct);
					if (holemodfail(p_x, p_y, orp)) {
#if 0
						printf("Culled a board:\n");
						printboard(depth);
#endif
						goto next;
					}

					move[depth].p_name = sp->std_name;
					move[depth].p_orient = or;
					move[depth].x = p_x;
					move[depth].y = p_y;

#if 0
					printboard(depth);
#endif

					board_tries++;
					if ((board_tries % 100000) == 0)
						printf("attempted %lu boards\n", board_tries);
					solve(depth + 1);

next:
					unplace(p_x, p_y, orp, &sp->ct);
				}
			}
		}
	}
}

int
main(int argc, char **argv)
{
	FILE *f;

	(void)printboard;

	if (argc < 2)
		errx(EX_USAGE, "solver: input");

	f = fopen(argv[1], "rb");
	if (f == NULL)
		err(EX_USAGE, "fopen: %s", argv[1]);

	parse(f);
	fclose(f);

	if (total_pc > ARRAY_LEN(move))
		errx(EX_OSERR, "%s: Expand move array", __func__);

	board_x = parse_lut[X_IDX].ct;
	board_y = parse_lut[Y_IDX].ct;

	board = malloc(board_x * board_y * sizeof(board[0]));
	if (board == NULL)
		err(EX_OSERR, "malloc");
	memset(board, 0, board_x * board_y * sizeof(board[0]));
	board_flood = malloc(board_x * board_y * sizeof(board_flood[0]));
	if (board_flood == NULL)
		err(EX_OSERR, "malloc");
	memset(board_flood, 0, board_x * board_y * sizeof(board_flood[0]));

	solve(0);

	printf("No solution\n");

	free(board);
	return (0);
}
