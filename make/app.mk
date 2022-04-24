## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

APP_DIR := $(BUILD)/$(APP_NAME)
APP_ELF := $(BUILD)/$(APP_NAME).elf
APP_LST := $(BUILD)/$(APP_NAME).lst
APP_LDSCRIPT := $(if $(APP_LDSCRIPT),$(APP_LDSCRIPT),$(LDSCRIPT))

ALL += $(APP_ELF) $(APP_LST)

# Generate objects from sources.
APP_OBJ := $(patsubst %.c,$(APP_DIR)/%.o,$(APP_SRC))
APP_OBJ := $(patsubst %.S,$(APP_DIR)/%.o,$(APP_OBJ))

# Assemble compile and link flags.
APP_CFLAGS := $(ARCHFLAGS) $(CFLAGS) $(patsubst %,-I%,$(APP_INC))
APP_LDFLAGS := $(ARCHFLAGS) -Wl,--gc-sections

$(shell mkdir -p $(APP_DIR))

OPTS.A := $(APP_CFLAGS) $(APP_LDFLAGS)
OPTS.B := $(file <$(APP_DIR)/build.opts)

ifneq ($(OPTS.A),$(OPTS.B))
$(info generating $(APP_DIR)/build.opts)
$(file >$(APP_DIR)/build.opts,$(OPTS.A))
endif

# Track flags in a build.opts file so we can depend on it.
# Write the file only if it does not already contain the
# same options as currently defined.
$(APP_OBJ): $(APP_DIR)/build.opts
$(APP_ELF): $(APP_DIR)/build.opts

$(APP_ELF): _OBJ := $(APP_OBJ)
$(APP_ELF): _LDFLAGS := $(APP_LDFLAGS) -T $(APP_LDSCRIPT)
$(APP_LIB): _LIB := -lgcc
$(APP_ELF): $(APP_OBJ) $(APP_LDSCRIPT) $(APP_DIR)/build.opts
	@$(info linking $@)
	$(V)$(XGCC) $(_LDFLAGS) -o $@ $(_OBJ) $(_LIB)

$(APP_LST): $(APP_ELF)
	$(V)$(XOBJDUMP) -D $< > $@

$(APP_DIR)/%.o: _CFLAGS := $(APP_CFLAGS)

$(APP_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(info compiling $<)
	$(V)$(XGCC) -c -o $@ $< $(_CFLAGS) -MD -MP -MT $@ -MF $(@:%o=%d)

$(APP_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(info compiling $<)
	$(V)$(XGCC) -c -o $@ $< $(_CFLAGS) -MD -MP -MT $@ -MF $(@:%o=%d)

# include compiler auto-deps
-include $(patsubst %.o,%.d,$(APP_OBJ))

run.$(APP_NAME):: $(QEMUBIOS)
run.$(APP_NAME):: _BIN := $(APP_ELF)
run.$(APP_NAME):: $(APP_ELF) $(APP_LST)
	$(QEMU) $(QFLAGS) -kernel $(_BIN)

debug.$(APP_NAME):: $(QEMUBIOS)
debug.$(APP_NAME):: _BIN := $(APP_ELF)
debug.$(APP_NAME):: $(APP_ELF) $(APP_LST)
	$(QEMU) $(QFLAGS.GDB) -kernel $(_BIN)

APP_NAME :=
APP_INC :=
APP_SRC :=
APP_LDSCRIPT :=
