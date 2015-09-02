#ifndef	_SOLVER_H_
#define	_SOLVER_H_

struct words {
	char str[32];
	unsigned ct;
	unsigned len;
	struct z_pc pc_zob;
};
extern struct words words[16];

#define	ARRAY_LEN(a)	(sizeof(a) / sizeof(a[0]))
#define	MIN(a, b)	({						\
	typeof(a) _min1 = (a);						\
	typeof(b) _min2 = (b);						\
	(void) (&_min1 == &_min2);					\
	_min1 < _min2 ? _min1 : _min2; })
#define	MAX(a, b)	({						\
	typeof(a) _min1 = (a);						\
	typeof(b) _min2 = (b);						\
	(void) (&_min1 == &_min2);					\
	_min1 < _min2 ? _min2 : _min1; })

extern uint8_t			*board;
extern unsigned			board_x, board_y;

#endif
