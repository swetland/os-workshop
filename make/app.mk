## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

MOD_DIR := $(BUILD)/$(MOD_NAME)
MOD_ELF := $(BUILD)/$(MOD_NAME).elf
MOD_LST := $(BUILD)/$(MOD_NAME).lst
MOD_BIN := $(BUILD)/$(MOD_NAME).bin
MOD_LDSCRIPT := $(if $(MOD_LDSCRIPT),$(MOD_LDSCRIPT),$(LDSCRIPT))
MOD_QFLAGS := $(if $(MOD_QEMU_FB),$(QFLAGS.FB),$(QFLAGS.TTY))

ALL += $(MOD_ELF) $(MOD_LST) $(MOD_BIN)

include make/rules.mk

$(MOD_ELF): $(MOD_DIR)/build.opts $(patsubst %,$(BUILD)/lib%.a,$(MOD_LIB))

$(MOD_ELF): _OBJ := $(MOD_OBJ)
$(MOD_ELF): _LDFLAGS := $(MOD_LDFLAGS) -T $(MOD_LDSCRIPT)
$(MOD_ELF): _LIB := -L$(BUILD) $(patsubst %,-l%,$(MOD_LIB)) -lgcc
$(MOD_ELF): $(MOD_OBJ) $(MOD_LDSCRIPT) 
	@$(info linking $@)
	$(V)$(XGCC) $(_LDFLAGS) -o $@ $(_OBJ) $(_LIB)

$(MOD_LST): $(MOD_ELF)
	$(V)$(XOBJDUMP) -d -z $< > $@

$(MOD_BIN): $(MOD_ELF)
	$(V)$(XOBJCOPY) -O binary $< $@

run.$(MOD_NAME):: _ELF := $(MOD_ELF)
run.$(MOD_NAME):: _QFLAGS := $(MOD_QFLAGS)
run.$(MOD_NAME):: $(MOD_ELF) $(MOD_LST) $(MOD_BIN) $(QEMUBIOS)
	$(QEMU) $(QFLAGS) $(_QFLAGS) -kernel $(_ELF)

debug.$(MOD_NAME):: _ELF := $(MOD_ELF)
run.$(MOD_NAME):: _QFLAGS := $(MOD_QFLAGS)
debug.$(MOD_NAME):: $(MOD_ELF) $(MOD_LST) $(MOD_BIN) $(QEMUBIOS)
	$(QEMU) $(QFLAGS.GDB) $(_QFLAGS) -kernel $(_ELF)

MOD_NAME :=
MOD_INC :=
MOD_SRC :=
MOD_LIB :=
MOD_LDSCRIPT :=
MOD_QEMU_FB :=
