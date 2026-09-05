# Shared settings for per-app Makefiles
# Expect TOP/ROOT to be passed from the top-level make when running in subdirs:
#   $(MAKE) -C examples/legacy_opengl ROOT=$(CURDIR) MODE=release
ROOT ?= $(CURDIR)
CC := gcc

CFLAGS := -W -Wall -Wextra -pedantic -pedantic-errors -Wconversion -Wdeprecated
DBGFLAGS := -O0 -g
RELFLAGS := -O3 -ffast-math -msse -msse2 -mfpmath=sse

# Linker flags for OpenGL apps (include math); per-app makefiles can override
LFLAGS_GL := -lGL -lglut -lGLU -lGLEW -lm

# Common paths (placed in repo root via ROOT)
BIN_DIR := $(ROOT)/bin
BUILD_DIR := $(ROOT)/build
INCLUDE_DIR := -I$(ROOT)/include
SCREENSHOT_DIR := $(ROOT)/screenshot
