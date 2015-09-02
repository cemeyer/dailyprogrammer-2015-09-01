WFLAGS=	-Wall -Wextra -Wno-missing-field-initializers -Werror

solver: main.c zobrist.c hash.c
	cc $(WFLAGS) -g -O3 -std=gnu11 -o $@ $^
