# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: MPL-2.0

include config.mk

SUBDIRS = benchmarks tests

SRCS = src/$(LIBNAME).c
HDRS = $(wildcard src/*.h) # Used for dependencies of the header-only target

LIB_ARTIFACTS = $(ANAME) $(SONAME_2) $(SONAME_1) $(SONAME_0)

ARTIFACTS = $(LIB_ARTIFACTS) $(SINGLE_HEADER) $(PCFILE)

CFLAGS += -fPIC -ffreestanding -g

all: $(ARTIFACTS) $(SUBDIRS)

$(SUBDIRS): $(LIB_ARTIFACTS)
	$(MAKE) -C $@

$(ANAME): $(OBJS)
	$(AR) $(ARFLAGS) $@ $^

$(SONAME_2): $(OBJS)
	$(CC) -o $@ -shared -nostdlib -Wl,-soname,$(SONAME_1) $(LDFLAGS) $^

$(SONAME_1): $(SONAME_2)
	@ln -s -f --verbose -- $< $@

$(SONAME_0): $(SONAME_2)
	@ln -s -f --verbose -- $< $@

# $(OBJS) has all the required dependencies for $(SINGLE_HEADER), but we
# needn't compile anything.
# This assumes all $(HDRS) are used by $(SRCS).
$(SINGLE_HEADER): $(HDRS) $(SRCS)
	@printf '#define RANDP_SINGLE_HEADER\n\n' > $@
	python3 amalgamate.py $(SRCS) >> $@

$(PCFILE): $(PCFILE).in
	@sed \
		-e 's|@PREFIX@|$(PREFIX)|g' \
		-e 's|@LIBDIR@|$(LIBDIR)|g' \
		-e 's|@INCDIR@|$(INCDIR)|g' \
		-e 's|@VERSION@|$(VERSION)|g' \
		-- $< > $@

# TODO: test this
install: $(ARTIFACTS)
	@install -D -v -m644 -- $(LIBNAME).h $(DESTDIR)$(INCDIR)/$(LIBNAME).h
	@install -D -v -m644 -- $(ANAME) $(DESTDIR)$(LIBDIR)/$(ANAME)
	@install -D -v -m755 -- $(SONAME_2) $(DESTDIR)$(LIBDIR)/$(SONAME_2)
	@ln -s -f --verbose -- $(SONAME_2) $(DESTDIR)$(LIBDIR)/$(SONAME_1)
	@ln -s -f --verbose -- $(SONAME_2) $(DESTDIR)$(LIBDIR)/$(SONAME_0)
	@install -D -v -m644 -- $(PCFILE) $(DESTDIR)$(PKGCONFIGDIR)/$(PCFILE)
	$(if $(DESTDIR),,@ldconfig --verbose -- $(LIBDIR))

# TODO: test this
uninstall:
	@$(RM) --verbose -- $(DESTDIR)$(INCDIR)/$(LIBNAME).h
	@$(RM) --verbose -- $(DESTDIR)$(LIBDIR)/$(ANAME)
	@$(RM) --verbose -- $(DESTDIR)$(LIBDIR)/$(SONAME_2)
	@$(RM) --verbose -- $(DESTDIR)$(LIBDIR)/$(SONAME_1)
	@$(RM) --verbose -- $(DESTDIR)$(LIBDIR)/$(SONAME_0)
	@$(RM) --verbose -- $(DESTDIR)$(PKGCONFIGDIR)/$(PCFILE)
	$(if $(DESTDIR),,@ldconfig --verbose)

clean:
	@$(RM) --verbose -- $(DEPS) $(OBJS) $(ARTIFACTS)
	for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

lint:
	-clang-tidy --quiet $(SRCS) -- $(CPPFLAGS) $(CFLAGS)
	-for dir in $(SUBDIRS); do $(MAKE) -C $$dir $@; done

test: $(LIB_ARTIFACTS)
	$(MAKE) -C tests

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all clean lint install uninstall test $(SUBDIRS) $(PCFILE)

# https://www.gnu.org/software/make/manual/html_node/Special-Targets.html#index-removing-targets-on-failure
.DELETE_ON_ERROR:

-include $(DEPS)
