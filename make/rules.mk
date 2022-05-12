## Copyright 2022, Brian Swetland <swetland@frotz.net>
## Licensed under the Apache License, Version 2.0

# Todo: check for missing inputs, duplicate module names, etc

# Assemble compile and link flags.
MOD_CFLAGS := $(ARCHFLAGS) $(CFLAGS) 
MOD_CFLAGS += $(patsubst %,-I%,$(MOD_INC)) $(patsubst %,-Ilib%/inc,$(MOD_LIB))
MOD_LDFLAGS := $(ARCHFLAGS) -Wl,--gc-sections

# Generate objects from sources.
MOD_OBJ := $(patsubst %.c,$(MOD_DIR)/%.o,$(MOD_SRC))
MOD_OBJ := $(patsubst %.S,$(MOD_DIR)/%.o,$(MOD_OBJ))

# Track flags in a build.opts file so we can depend on it.
# Write the file only if it does not already contain the
# same options as currently defined.
$(shell mkdir -p $(MOD_DIR))

OPTS.A := $(strip $(MOD_CFLAGS) $(MOD_LDFLAGS))
OPTS.B := $(strip $(file <$(MOD_DIR)/build.opts))

ifneq ($(OPTS.A),$(OPTS.B))
$(info generating $(MOD_DIR)/build.opts)
$(file >$(MOD_DIR)/build.opts,$(OPTS.A))
endif

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

$(MOD_OBJ): $(MOD_DIR)/build.opts

