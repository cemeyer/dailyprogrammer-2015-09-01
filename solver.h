#ifndef	_SOLVER_H_
#define	_SOLVER_H_

struct z_pc;
struct str_to_piece {
	const char *str;
	const char *std_name;
	const struct piece *pc;

	unsigned ct;
	struct z_pc pc_zob;
};

#define	ARRAY_LEN(a)	(sizeof(a) / sizeof(a[0]))
#define	MIN(a, b)	({						\
	typeof(a) _min1 = (a);						\
	typeof(b) _min2 = (b);						\
	(void) (&_min1 == &_min2);					\
	_min1 < _min2 ? _min1 : _min2; })

extern uint8_t			*board;
extern struct str_to_piece	parse_lut[];
extern unsigned			board_x, board_y;

#endif
