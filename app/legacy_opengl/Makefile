include ../../common.mk

APP := legacy_opengl
SRC := $(APP).c

ifeq ($(MODE),debug)
OBJ := $(BUILD_DIR)/$(APP)_d.o
BIN := $(BIN_DIR)/$(APP)_d
FLAGS := $(DBGFLAGS)
else
OBJ := $(BUILD_DIR)/$(APP).o
BIN := $(BIN_DIR)/$(APP)
FLAGS := $(RELFLAGS)
endif

.PHONY: all clean
all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p $(@D)
	$(CC) -o $@ $^ $(LFLAGS_GL)

$(OBJ): $(SRC) $(ROOT)/include/trayracing/trayracing.h
	@mkdir -p $(@D)
	$(CC) -o $@ -c $< $(CFLAGS) $(FLAGS) -Wa,-adhln -fverbose-asm -DSCREENSHOTS_FOLDER=\"$(SCREENSHOT_DIR)\" $(INCLUDE_DIR) > $@.s

clean:
	@rm -f $(OBJ) $(BIN)
