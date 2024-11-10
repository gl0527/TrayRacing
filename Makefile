CC := gcc
CFLAGS := -W -Wall -Wextra -pedantic -pedantic-errors -Wconversion -Wdeprecated
LFLAGS := -lGL -lglut -lm -lGLU -lGLEW

.PHONY: all clean

all: tr_dbg tr_rel

tr_dbg: main.c trayracing.h
	@$(CC) -o $@ $^ $(CFLAGS) -O0 -g $(LFLAGS)

tr_rel: main.c trayracing.h
	@$(CC) -o $@ $^ $(CFLAGS) -O3 -ffast-math -msse -msse2 -mfpmath=sse $(LFLAGS)

clean:
	@rm -f tr_dbg tr_rel
