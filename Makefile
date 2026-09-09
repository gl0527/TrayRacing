# Top-level orchestrator: delegates build to per-app makefiles
.PHONY: all debug release clean screenshot legacy_opengl nogui ascii

all: debug release screenshot

debug: legacy_opengl_d nogui_d ascii_d
release: legacy_opengl nogui ascii

# legacy_opengl
legacy_opengl:
	$(MAKE) -C app/legacy_opengl ROOT=$(CURDIR) MODE=release
legacy_opengl_d:
	$(MAKE) -C app/legacy_opengl ROOT=$(CURDIR) MODE=debug

# nogui
nogui:
	$(MAKE) -C app/nogui ROOT=$(CURDIR) MODE=release
nogui_d:
	$(MAKE) -C app/nogui ROOT=$(CURDIR) MODE=debug

# ascii
ascii:
	$(MAKE) -C app/ascii ROOT=$(CURDIR) MODE=release
ascii_d:
	$(MAKE) -C app/ascii ROOT=$(CURDIR) MODE=debug

screenshot:
	@mkdir -p $(CURDIR)/screenshot

clean:
	-$(MAKE) -C app/legacy_opengl clean || true
	-$(MAKE) -C app/nogui clean || true
	-$(MAKE) -C app/ascii clean || true
	@rm -rf $(CURDIR)/build $(CURDIR)/bin $(CURDIR)/screenshot
