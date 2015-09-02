#ifndef	_ZOBRIST_H_
#define	_ZOBRIST_H_

struct words;

struct s_z128 {
	uint64_t	v[2];
};
typedef struct s_z128 z128_t;

struct z_pc {
	z128_t		*values;
	unsigned	n;
};

void zob_init_first(void);
void zob_board_init(unsigned x, unsigned y);
void zob_piece_init(struct words *);
bool zob_seen_this(void);
void zob_record_this(void);

#endif
