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

ifneq ($(MOD_EXT),)
# if there is an extra data file, arrange for it to
# be added as an object file containing an .extra
# segment with the raw data.  The linker script
# will stuff it in .rodata sandwiched between
# __extra_start and __extra_end symbols
$(BUILD)/$(MOD_NAME)/extra.o: $(MOD_EXT)
	$(V)$(XOBJCOPY) -I binary -O elf32-littleriscv --rename-section .data=.extra $< $@

MOD_OBJ += $(BUILD)/$(MOD_NAME)/extra.o
endif

$(MOD_ELF): $(MOD_DIR)/build.opts $(patsubst %,$(BUILD)/lib%.a,$(MOD_LIB)) 

$(MOD_ELF): _OBJ := $(MOD_OBJ)
$(MOD_ELF): _LDFLAGS := $(MOD_LDFLAGS) -T $(MOD_LDSCRIPT)
$(MOD_ELF): _LIB := -L$(BUILD) $(patsubst %,-l%,$(MOD_LIB)) -lgcc
$(MOD_ELF): $(MOD_OBJ) $(MOD_LDSCRIPT) make/common.ram.ld
	@$(info linking $@)
	$(V)$(XGCC) $(_LDFLAGS) -o $@ $(_OBJ) $(_LIB)

$(MOD_LST): $(MOD_ELF)
	$(V)$(XOBJDUMP) -d -z $< > $@

$(MOD_BIN): $(MOD_ELF)
	$(V)$(XOBJCOPY) -O binary $< $@

run.$(MOD_NAME):: _BIN := $(MOD_BIN)
run.$(MOD_NAME):: _QFLAGS := $(MOD_QFLAGS)
run.$(MOD_NAME):: $(MOD_ELF) $(MOD_LST) $(MOD_BIN) $(QEMUBIOS)
	$(QEMU) $(QFLAGS) $(_QFLAGS) -kernel $(_BIN)

debug.$(MOD_NAME):: _BIN := $(MOD_BIN)
run.$(MOD_NAME):: _QFLAGS := $(MOD_QFLAGS)
debug.$(MOD_NAME):: $(MOD_ELF) $(MOD_LST) $(MOD_BIN) $(QEMUBIOS)
	$(QEMU) $(QFLAGS.GDB) $(_QFLAGS) -kernel $(_BIN)

MOD_NAME :=
MOD_INC :=
MOD_SRC :=
MOD_LIB :=
MOD_EXT :=
MOD_LDSCRIPT :=
MOD_QEMU_FB :=
