## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

MOD_DIR := $(BUILD)/lib$(MOD_NAME)
MOD_LIB := $(BUILD)/lib$(MOD_NAME).a

ALL += $(MOD_LIB)

include make/rules.mk

$(MOD_LIB): $(MOD_DIR)/build.opts 

$(MOD_LIB): _OBJ := $(MOD_OBJ)
$(MOD_LIB): $(MOD_OBJ) $(MOD_DIR)/build.opts
	@$(info linking $@)
	@rm -f $@
	$(V)$(XAR) -crs $@ -o $@ $(_OBJ)

MOD_NAME :=
MOD_INC :=
MOD_SRC :=
MOD_LIB :=
MOD_LDSCRIPT :=
