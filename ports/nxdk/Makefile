XBE_TITLE = Slime\ Volley
NXDK_DIR ?= $(CURDIR)/../../.local/nxdk
NXDK_SDL = y
ROOT := $(CURDIR)/../..
GAME_SRCS := $(addprefix $(ROOT)/src/,audio.c clavier.c credits.c IA.c jeu.c menu_jeu.c menu_options.c menu_princ.c menu.c objet.c slime.c themes_general.c themes_std.c echelle.c)
SRCS := $(GAME_SRCS) $(CURDIR)/Platform.c
CFLAGS += -O2 -fcommon -I$(CURDIR)/include -I$(ROOT)/src -I$(CURDIR)
PORT_CONFIG_FLAGS := $(CFLAGS)
$(addsuffix .obj,$(basename $(GAME_SRCS))): CFLAGS += -include $(CURDIR)/Compat.h
ifdef SOURCE_DATE_EPOCH
LDFLAGS += /timestamp:$(SOURCE_DATE_EPOCH)
endif
include $(NXDK_DIR)/Makefile

.PHONY: port-config-force
$(CURDIR)/.build-config: port-config-force
	@printf '%s\n' '$(PORT_CONFIG_FLAGS) $(NXDK_CFLAGS)' '$(LDFLAGS) $(NXDK_LDFLAGS)' 'SOURCE_DATE_EPOCH=$(SOURCE_DATE_EPOCH)' > '$@.tmp'
	@cmp -s '$@.tmp' '$@' || mv '$@.tmp' '$@'
	@rm -f '$@.tmp'
$(OBJS): $(CURDIR)/.build-config
