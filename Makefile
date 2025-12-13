CC := gcc
CFLAGS := -W -Wall -Wextra -pedantic -pedantic-errors -Wconversion -Wdeprecated
LFLAGS := -lGL -lglut -lm -lGLU -lGLEW
INCLUDE_FOLDER := $(CURDIR)/include
EXAMPLES_FOLDER := $(CURDIR)/examples
BIN_FOLDER := $(CURDIR)/bin
SCREENSHOTS_FOLDER := $(CURDIR)/screenshots

.PHONY: all clean

all: $(BIN_FOLDER)/ogl_dbg $(BIN_FOLDER)/ogl_rel $(SCREENSHOTS_FOLDER)

$(BIN_FOLDER)/ogl_dbg: $(EXAMPLES_FOLDER)/main.c $(INCLUDE_FOLDER)/trayracing/trayracing.h
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(CFLAGS) -I$(INCLUDE_FOLDER) -O0 -g $(LFLAGS)

$(BIN_FOLDER)/ogl_rel: $(EXAMPLES_FOLDER)/main.c $(INCLUDE_FOLDER)/trayracing/trayracing.h
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(CFLAGS) -I$(INCLUDE_FOLDER) -O3 -ffast-math -msse -msse2 -mfpmath=sse $(LFLAGS)

$(SCREENSHOTS_FOLDER):
	@mkdir -p $(SCREENSHOTS_FOLDER)

clean:
	@rm -rf $(BIN_FOLDER) $(SCREENSHOTS_FOLDER)
