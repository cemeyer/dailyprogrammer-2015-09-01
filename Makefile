# -std=gnu11 for typeof() in MIN().  Otherwise it can be -std=c11.
# Drop pedantic for MIN() ({ }) as well :(.
WFLAGS=	-Wall -Wextra -Wno-missing-field-initializers -Werror
solver: main.c
	cc $(WFLAGS) -g -O3 -std=gnu11 -o $@ $^
