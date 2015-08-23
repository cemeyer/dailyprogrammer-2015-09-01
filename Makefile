WFLAGS=	-Wall -Wextra -pedantic -Wno-missing-field-initializers -Werror

solver: main.c
	cc $(WFLAGS) -g -O3 -std=c11 -o $@ $^
