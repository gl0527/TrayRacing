# Top-level orchestrator: delegates build to per-app makefiles
.PHONY: all debug release clean screenshots legacy_opengl nogui

all: debug release screenshots

debug: legacy_opengl_d nogui_d
release: legacy_opengl nogui

# legacy_opengl
legacy_opengl:
	$(MAKE) -C examples/legacy_opengl ROOT=$(CURDIR) MODE=release
legacy_opengl_d:
	$(MAKE) -C examples/legacy_opengl ROOT=$(CURDIR) MODE=debug

# nogui
nogui:
	$(MAKE) -C examples/nogui ROOT=$(CURDIR) MODE=release
nogui_d:
	$(MAKE) -C examples/nogui ROOT=$(CURDIR) MODE=debug

screenshots:
	@mkdir -p $(CURDIR)/screenshots

clean:
	-$(MAKE) -C examples/legacy_opengl clean || true
	-$(MAKE) -C examples/nogui clean || true
	@rm -rf $(CURDIR)/build $(CURDIR)/bin $(CURDIR)/screenshots
