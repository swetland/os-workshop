## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

MOD_DIR := $(BUILD)/lib$(MOD_NAME)
MOD_LIB += $(MOD_NAME)
MOD_ALIB := $(BUILD)/lib$(MOD_NAME).a

ALL += $(MOD_ALIB)

include make/rules.mk

$(MOD_ALIB): $(MOD_DIR)/build.opts 

$(MOD_ALIB): _OBJ := $(MOD_OBJ)
$(MOD_ALIB): $(MOD_OBJ) $(MOD_DIR)/build.opts
	@$(info linking $@)
	@rm -f $@
	$(V)$(XAR) -crs $@ -o $@ $(_OBJ)

MOD_NAME :=
MOD_INC :=
MOD_SRC :=
MOD_LIB :=
MOD_LDSCRIPT :=
