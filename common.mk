# Shared settings for per-app Makefiles.
# Per-app builds are invoked with ROOT set to the repository root.
ROOT ?= $(CURDIR)
CC := gcc

CFLAGS := -W -Wall -Wextra -pedantic -pedantic-errors -Wconversion -Wdeprecated
DBGFLAGS := -O0 -g
RELFLAGS := -O3 -ffast-math -msse -msse2 -mfpmath=sse

# Linker flags for OpenGL apps (include math); per-app makefiles can override.
LFLAGS_GL := -lGL -lglut -lGLU -lGLEW -lm

# Common paths.
BIN_DIR := $(ROOT)/bin
BUILD_DIR := $(ROOT)/build
INCLUDE_DIR := -I$(ROOT)/include
SCREENSHOT_DIR := $(ROOT)/screenshot/

# Generate static diffuse maps from PNG/JPG inputs. Generated headers are
# included as object prerequisites so they are created before compilation.
TEXTURE_DIR := $(ROOT)/asset/texture
GEN_DIR := $(ROOT)/include/trayracing/generated
GENERATOR := $(ROOT)/tool/generate_diffuse_map.py
TEXTURES := $(wildcard $(TEXTURE_DIR)/*.png) $(wildcard $(TEXTURE_DIR)/*.jpg)
GENERATED_HEADERS := $(foreach texture,$(TEXTURES),$(GEN_DIR)/DiffuseMap$(basename $(notdir $(texture))).h)

.PHONY: generate-textures

generate-textures: $(GENERATED_HEADERS)

$(GEN_DIR):
	@mkdir -p $@

$(GEN_DIR)/DiffuseMap%.h: $(TEXTURE_DIR)/%.png $(GENERATOR) | $(GEN_DIR)
	python3 $(GENERATOR) $< $@

$(GEN_DIR)/DiffuseMap%.h: $(TEXTURE_DIR)/%.jpg $(GENERATOR) | $(GEN_DIR)
	python3 $(GENERATOR) $< $@
