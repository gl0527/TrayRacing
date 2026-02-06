CC := gcc

CFLAGS := -W -Wall -Wextra -pedantic -pedantic-errors -Wconversion -Wdeprecated
DBGFLAGS := -O0 -g
RELFLAGS := -O3 -ffast-math -msse -msse2 -mfpmath=sse
LFLAGS := -lGL -lglut -lm -lGLU -lGLEW

INCLUDE_FOLDER := $(CURDIR)/include/
EXAMPLES_FOLDER := $(CURDIR)/examples/
BIN_FOLDER := $(CURDIR)/bin/
BUILD_FOLDER := $(CURDIR)/build/
SCREENSHOTS_FOLDER := $(CURDIR)/screenshots/

.PHONY: all debug release clean

all: debug release $(SCREENSHOTS_FOLDER)

debug: $(BUILD_FOLDER)ogl_dbg.o $(BIN_FOLDER)ogl_dbg
release: $(BUILD_FOLDER)ogl_rel.o $(BIN_FOLDER)ogl_rel

$(BIN_FOLDER)%: $(BUILD_FOLDER)%.o
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LFLAGS)

$(BUILD_FOLDER)ogl_dbg.o: $(EXAMPLES_FOLDER)legacy_opengl.c $(INCLUDE_FOLDER)trayracing/trayracing.h
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS) $(DBGFLAGS) -Wa,-adhln -fverbose-asm -DSCREENSHOTS_FOLDER=\"$(SCREENSHOTS_FOLDER)\" -I$(INCLUDE_FOLDER) > $(BUILD_FOLDER)ogl_dbg.s

$(BUILD_FOLDER)ogl_rel.o: $(EXAMPLES_FOLDER)legacy_opengl.c $(INCLUDE_FOLDER)trayracing/trayracing.h
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS) $(RELFLAGS) -Wa,-adhln -fverbose-asm -DSCREENSHOTS_FOLDER=\"$(SCREENSHOTS_FOLDER)\" -I$(INCLUDE_FOLDER) > $(BUILD_FOLDER)ogl_rel.s

$(SCREENSHOTS_FOLDER):
	@mkdir -p $(SCREENSHOTS_FOLDER)

clean:
	@rm -rf $(BIN_FOLDER) $(BUILD_FOLDER) $(SCREENSHOTS_FOLDER)
