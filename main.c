#include <ctype.h>
#include <err.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include "zobrist.h"
#include "solver.h"

static unsigned total_pc;
struct words words[16];

static void
parse_set(const char *what)
{
	struct words *sp;

	for (sp = words; sp->ct != 0; sp++) {
		if (strcmp(sp->str, what) == 0)
			break;
	}

	if (sp->ct == 0) {
		strcpy(sp->str, what);
		sp->len = strlen(sp->str);
	}
	sp->ct++;
	total_pc++;

	board_x = MAX(board_x, sp->len * 3);
	board_y = MAX(board_y, sp->len * 2);
}

char astr[128];
static void
parse(FILE *f)
{
	struct words *sp;
	unsigned word;
	int n;

	memset(astr, 0, sizeof(astr));

	for (word = 1;; word++) {
		n = fscanf(f, "%127[a-z'-], ", astr);
		if (n == EOF)
			break;

		if (n < 1)
			errx(EX_USAGE, "failed to parse at word: %u", word);

		parse_set(astr);
	}

	for (sp = words; sp->ct != 0; sp++)
		zob_piece_init(sp);
}

#define	OR_HOR	0
#define	OR_VER	1
struct move {
	const char *p_name;
	unsigned p_orient, x, y;
};

static struct move move[128];
unsigned board_x, board_y, total_match;
static uint64_t board_tries;
uint8_t *board, *match;

static struct move movebest[128];
static unsigned bestscore;

#define	BOARD_SQ(x, y)	board[ board_x * (y) + (x) ]
#define	MATCH_SQ(x, y)	match[ board_x * (y) + (x) ]

enum plhow {
	PLACE_TRY,
	PLACE_DO,
	PLACE_UNDO,
};

struct piece_or {
	struct words *wp;
	unsigned x, y,	/* w x h */
		 or;
};

static char
orp_letter(const struct piece_or *orp, unsigned x, unsigned y)
{

	if (orp->or == OR_HOR) {
		if (orp->wp->len <= x)
			err(1, "dumb %d", __LINE__);
		return (toupper(orp->wp->str[x]));
	} else {
		if (orp->wp->len <= y)
			err(1, "dumb %d", __LINE__);
		return (toupper(orp->wp->str[y]));
	}
}

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
			pix_x = x + spr_x;
			pix_y = y + spr_y;

			if (BOARD_SQ(pix_x, pix_y) &&
			    BOARD_SQ(pix_x, pix_y) != orp_letter(orp, spr_x, spr_y)) {
				// already set, doesn't match
				if (how == PLACE_TRY)
					return (false);
				else if (how == PLACE_DO)
					errx(EX_OSERR, "i dumb %d", __LINE__);
			} else if (BOARD_SQ(pix_x, pix_y)) {
				// already set, does match
				if (how == PLACE_UNDO) {
					if (MATCH_SQ(pix_x, pix_y)) {
						total_match--;
						MATCH_SQ(pix_x, pix_y) = 0;
					} else
						BOARD_SQ(pix_x, pix_y) = 0;
				} else if (how == PLACE_DO) {
					//BOARD_SQ(pix_x, pix_y) = orp_letter(orp, x, y);
					MATCH_SQ(pix_x, pix_y)++;
					total_match++;
				}
			} else {
				// square was empty
				if (how == PLACE_DO)
					BOARD_SQ(pix_x, pix_y) = orp_letter(orp, spr_x, spr_y);
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
printboard(unsigned sc, unsigned depth)
{
	unsigned x, y;

#if 0
	static unsigned print_count;
	print_count++;
	if (print_count > 10)
		errx(0, "%s: done spamming", __func__);
#endif

	printf("\nBoard @ depth=%u score=%u (piece remain=%u)\n", depth, sc,
	    total_pc);
	for (y = 0; y < board_y; y++) {
		for (x = 0; x < board_x; x++) {
			if (board[board_x * y + x] != 0)
				putchar((char)board[board_x * y + x]);
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
win(unsigned depth)
{

	printboard(total_match, depth);
	fflush(stdout);

	memcpy(movebest, move, sizeof(movebest));
	bestscore = total_match;
}

static void
solve(unsigned depth)
{
	struct words *sp;

	const struct piece_or *orp;
	unsigned or;
	unsigned or_x, or_y;
	unsigned p_x, p_y;

	unsigned score_begin;

	if (depth >= ARRAY_LEN(move))
		errx(EX_OSERR, "i dumb");

	/* If we're out of pieces, print successful placements and exit */
	if (total_pc == 0 && total_match > bestscore) {
		win(depth);
		return;
	}

	if (depth > 1 && zob_seen_this())
		return;

	score_begin = total_match;

	/* If we can, play a piece */
	for (sp = words; *sp->str != '\0'; sp++) {
		if (sp->ct == 0)
			continue;

		for (or = 0; or < 2; or++) {
			struct piece_or por;

			por.wp = sp;
			por.or = or;
			if (or == OR_HOR) {
				por.x = sp->len;
				por.y = 1;
			} else {
				por.x = 1;
				por.y = sp->len;
			}
			orp = &por;

			or_x = orp->x;
			or_y = orp->y;

			for (p_x = 0; p_x + or_x <= board_x; p_x++) {
				for (p_y = 0; p_y + or_y <= board_y; p_y++) {
					if (!canplace(p_x, p_y, orp))
						continue;

					place(p_x, p_y, orp, &sp->ct);

					/* Skip placements, after the first, that aren't matches */
					if (depth > 0 && total_match == score_begin)
						goto next;

					move[depth].p_name = sp->str;
					move[depth].p_orient = or;
					move[depth].x = p_x;
					move[depth].y = p_y;

#if 0
					printboard(total_match, depth);
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

	zob_record_this();
}

int
main(int argc, char **argv)
{
	FILE *f;

	(void)printboard;

	if (argc < 2)
		err(1, "inp");

	zob_init_first();

	f = fopen(argv[1], "rb");
	parse(f);
	fclose(f);

	if (total_pc > ARRAY_LEN(move))
		errx(EX_OSERR, "%s: Expand move array", __func__);

	zob_board_init(board_x, board_y);

	board = malloc(board_x * board_y * sizeof(board[0]));
	if (board == NULL)
		err(EX_OSERR, "malloc");
	memset(board, 0, board_x * board_y * sizeof(board[0]));

	match = malloc(board_x * board_y * sizeof(board[0]));
	if (match == NULL)
		err(EX_OSERR, "malloc");
	memset(match, 0, board_x * board_y * sizeof(board[0]));

#if 1
	printboard(total_match, 0);
#endif
	solve(0);

	free(board);
	free(match);
	return (0);
}
