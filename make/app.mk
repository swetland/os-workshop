## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

APPDIR := $(BUILD)/$(APP)
APPELF := $(BUILD)/$(APP).elf
APPLST := $(BUILD)/$(APP).lst

ALL += $(APPELF) $(APPLST)

# Generate objects from sources.
APPOBJ := $(patsubst %.c,$(APPDIR)/%.o,$(SRC))
APPOBJ := $(patsubst %.S,$(APPDIR)/%.o,$(APPOBJ))

# Assemble compile and link flags.
APPCFLAGS := $(ARCHFLAGS) $(CFLAGS) $(patsubst %,-I%,$(APPINC))
APPLDFLAGS := $(ARCHFLAGS) -Wl,--gc-sections

$(shell mkdir -p $(APPDIR))

# Track flags in a build.opts file so we can depend on it.
# Write the file only if it does not already contain the
# same options as currently defined.
$(APPOBJ): $(APPDIR)/build.opts
$(APPELF): $(APPDIR)/build.opts

OPTS.A := $(APPCFLAGS) $(APPLDFLAGS)
OPTS.B := $(file <$(APPDIR)/build.opts)

ifneq ($(OPTS.A),$(OPTS.B))
$(file >$(APPDIR)/build.opts,$(OPTS.A))
endif

$(APPELF): _OBJ := $(APPOBJ)
$(APPELF): _LDFLAGS := $(APPLDFLAGS) -T $(LDSCRIPT)
$(APPLIB): _LIB := -lgcc
$(APPELF): $(APPOBJ) $(LDSCRIPT) $(APPDIR)/build.opts
	@$(info linking $@)
	$(V)$(XGCC) $(_LDFLAGS) -o $@ $(_OBJ) $(_LIB)

$(APPLST): $(APPELF)
	$(V)$(XOBJDUMP) -D $< > $@

$(APPDIR)/%.o: _CFLAGS := $(APPCFLAGS)

$(APPDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(info compiling $<)
	$(V)$(XGCC) -c -o $@ $< $(_CFLAGS) -MD -MP -MT $@ -MF $(@:%o=%d)

$(APPDIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(info compiling $<)
	$(V)$(XGCC) -c -o $@ $< $(_CFLAGS) -MD -MP -MT $@ -MF $(@:%o=%d)

-include $(patsubst %.o,%.d,$(APPOBJ))

run.$(APP):: _BIN := $(APPELF)
run.$(APP):: $(APPELF) $(APPLST)
	$(QEMU) $(QFLAGS) -kernel $(_BIN)

debug.$(APP):: _BIN := $(APPELF)
debug.$(APP):: $(APPELF) $(APPLST)
	$(QEMU) $(QFLAGS.GDB) -kernel $(_BIN)

APP :=
INC :=
SRC :=

