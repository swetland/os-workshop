## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

MOD_DIR := $(BUILD)/$(MOD_NAME)
MOD_ELF := $(BUILD)/$(MOD_NAME).elf
MOD_LST := $(BUILD)/$(MOD_NAME).lst
MOD_BIN := $(BUILD)/$(MOD_NAME).bin
MOD_LDSCRIPT := $(if $(MOD_LDSCRIPT),$(MOD_LDSCRIPT),$(LDSCRIPT))

ALL += $(MOD_ELF) $(MOD_LST) $(MOD_BIN)

# Generate objects from sources.
MOD_OBJ := $(patsubst %.c,$(MOD_DIR)/%.o,$(MOD_SRC))
MOD_OBJ := $(patsubst %.S,$(MOD_DIR)/%.o,$(MOD_OBJ))

# Assemble compile and link flags.
MOD_CFLAGS := $(ARCHFLAGS) $(CFLAGS) $(patsubst %,-I%,$(MOD_INC))
MOD_LDFLAGS := $(ARCHFLAGS) -Wl,--gc-sections

$(shell mkdir -p $(MOD_DIR))

OPTS.A := $(MOD_CFLAGS) $(MOD_LDFLAGS)
OPTS.B := $(file <$(MOD_DIR)/build.opts)

ifneq ($(OPTS.A),$(OPTS.B))
$(info generating $(MOD_DIR)/build.opts)
$(file >$(MOD_DIR)/build.opts,$(OPTS.A))
endif

# Track flags in a build.opts file so we can depend on it.
# Write the file only if it does not already contain the
# same options as currently defined.
$(MOD_OBJ): $(MOD_DIR)/build.opts
$(MOD_ELF): $(MOD_DIR)/build.opts

$(MOD_ELF): _OBJ := $(MOD_OBJ)
$(MOD_ELF): _LDFLAGS := $(MOD_LDFLAGS) -T $(MOD_LDSCRIPT)
$(MOD_LIB): _LIB := -lgcc
$(MOD_ELF): $(MOD_OBJ) $(MOD_LDSCRIPT) $(MOD_DIR)/build.opts
	@$(info linking $@)
	$(V)$(XGCC) $(_LDFLAGS) -o $@ $(_OBJ) $(_LIB)

$(MOD_LST): $(MOD_ELF)
	$(V)$(XOBJDUMP) -D $< > $@

$(MOD_BIN): $(MOD_ELF)
	$(V)$(XOBJCOPY) -O binary $< $@

$(MOD_DIR)/%.o: _CFLAGS := $(MOD_CFLAGS)

$(MOD_DIR)/%.o: %.c
	@mkdir -p $(dir $@)
	@$(info compiling $<)
	$(V)$(XGCC) -c -o $@ $< $(_CFLAGS) -MD -MP -MT $@ -MF $(@:%o=%d)

$(MOD_DIR)/%.o: %.S
	@mkdir -p $(dir $@)
	@$(info compiling $<)
	$(V)$(XGCC) -c -o $@ $< $(_CFLAGS) -D__ASSEMBLY__ -MD -MP -MT $@ -MF $(@:%o=%d)

# include compiler auto-deps
-include $(patsubst %.o,%.d,$(MOD_OBJ))

run.$(MOD_NAME):: $(QEMUBIOS)
run.$(MOD_NAME):: _BIN := $(MOD_ELF)
run.$(MOD_NAME):: $(MOD_ELF) $(MOD_LST)
	$(QEMU) $(QFLAGS) -kernel $(_BIN)

debug.$(MOD_NAME):: $(QEMUBIOS)
debug.$(MOD_NAME):: _BIN := $(MOD_ELF)
debug.$(MOD_NAME):: $(MOD_ELF) $(MOD_LST)
	$(QEMU) $(QFLAGS.GDB) -kernel $(_BIN)

MOD_NAME :=
MOD_INC :=
MOD_SRC :=
MOD_LDSCRIPT :=
