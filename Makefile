# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

include config.mk

SRCS := src/$(LIBNAME).c

CFLAGS += -fPIC -ffreestanding -g

all: $(ANAME) $(SONAME_2) $(SONAME_1) $(SONAME_0)

$(ANAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $<

$(SONAME_2): $(OBJS)
	$(CC) -o $@ -shared -nostdlib -Wl,-soname,$(SONAME_1) $(LDFLAGS) $<

$(SONAME_1): $(SONAME_2)
	@ln -s -f --verbose -- $< $@

$(SONAME_0): $(SONAME_2)
	@ln -s -f --verbose -- $< $@

clean:
	@$(RM) --verbose -- $(DEPS) $(OBJS) $(ANAME) $(SONAME_2) $(SONAME_1) $(SONAME_0)

lint:
	-clang-tidy --quiet $(SRCS) -- $(CPPFLAGS) $(CFLAGS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all clean lint

-include $(DEPS)
