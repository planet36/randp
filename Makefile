# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

include config.mk

SRCS := src/$(LIBNAME).c

CFLAGS += -fPIC -ffreestanding -g

all: $(ANAME) $(SONAME_2) $(SONAME_1) $(SONAME_0) $(SINGLE_HEADER)

$(ANAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $<

$(SONAME_2): $(OBJS)
	$(CC) -o $@ -shared -nostdlib -Wl,-soname,$(SONAME_1) $(LDFLAGS) $<

$(SONAME_1): $(SONAME_2)
	@ln -s -f --verbose -- $< $@

$(SONAME_0): $(SONAME_2)
	@ln -s -f --verbose -- $< $@

$(SINGLE_HEADER): $(OBJS)
	@printf "#define RANDP_SINGLE_HEADER\n" > $@
	python3 amalgamate.py $(SRCS) >> $@

# TODO: test this
install: all
	@mkdir -p --verbose -- $(INCDIR)
	@mkdir -p --verbose -- $(LIBDIR)
	@cp -f --verbose -- $(LIBNAME).h $(INCDIR)
	@cp -f --verbose -- $(ANAME) $(LIBDIR)
	@cp -f --verbose -- $(SONAME_2) $(LIBDIR)
	@cp -f --verbose -- $(SONAME_1) $(LIBDIR)
	@cp -f --verbose -- $(SONAME_0) $(LIBDIR)
	@ldconfig --verbose -- $(LIBDIR)

# TODO: test this
uninstall:
	@$(RM) --verbose -- $(INCDIR)/$(LIBNAME).h
	@$(RM) --verbose -- $(LIBDIR)/$(ANAME)
	@$(RM) --verbose -- $(LIBDIR)/$(SONAME_2)
	@$(RM) --verbose -- $(LIBDIR)/$(SONAME_1)
	@$(RM) --verbose -- $(LIBDIR)/$(SONAME_0)
	@ldconfig --verbose

clean:
	@$(RM) --verbose -- $(DEPS) $(OBJS) $(ANAME) $(SONAME_2) $(SONAME_1) $(SONAME_0) $(SINGLE_HEADER)

lint:
	-clang-tidy --quiet $(SRCS) -- $(CPPFLAGS) $(CFLAGS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all clean lint

-include $(DEPS)
