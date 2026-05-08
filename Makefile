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

debug: legacy_opengl_d nogui_d
release: legacy_opengl nogui

legacy_opengl_d: $(BUILD_FOLDER)legacy_opengl_d.o $(BIN_FOLDER)legacy_opengl_d
legacy_opengl: $(BUILD_FOLDER)legacy_opengl.o $(BIN_FOLDER)legacy_opengl

nogui_d: $(BUILD_FOLDER)nogui_d.o $(BIN_FOLDER)nogui_d
nogui: $(BUILD_FOLDER)nogui.o $(BIN_FOLDER)nogui

$(BIN_FOLDER)%: $(BUILD_FOLDER)%.o
	@mkdir -p $(@D)
	@$(CC) -o $@ $^ $(LFLAGS)

$(BUILD_FOLDER)%_d.o: $(EXAMPLES_FOLDER)%.c $(INCLUDE_FOLDER)trayracing/trayracing.h
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS) $(DBGFLAGS) -Wa,-adhln -fverbose-asm -DSCREENSHOTS_FOLDER=\"$(SCREENSHOTS_FOLDER)\" -I$(INCLUDE_FOLDER) > $@.s

$(BUILD_FOLDER)%.o: $(EXAMPLES_FOLDER)%.c $(INCLUDE_FOLDER)trayracing/trayracing.h
	@mkdir -p $(@D)
	@$(CC) -o $@ -c $< $(CFLAGS) $(RELFLAGS) -Wa,-adhln -fverbose-asm -DSCREENSHOTS_FOLDER=\"$(SCREENSHOTS_FOLDER)\" -I$(INCLUDE_FOLDER) > $@.s

$(SCREENSHOTS_FOLDER):
	@mkdir -p $(SCREENSHOTS_FOLDER)

clean:
	@rm -rf $(BIN_FOLDER) $(BUILD_FOLDER) $(SCREENSHOTS_FOLDER)
