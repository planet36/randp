# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: OSL-3.0

export LC_ALL := C

VERSION_MAJOR := 1
VERSION_MINOR := 0

LIBNAME := arp
ANAME := lib$(LIBNAME).a
# "versionless" soname
SONAME_0 := lib$(LIBNAME).so
# soname
SONAME_1 := $(SONAME_0).$(VERSION_MAJOR)
# "realname"
SONAME_2 := $(SONAME_1).$(VERSION_MINOR)

SRC := src/$(LIBNAME).c
OBJ := $(SRC:.c=.o)

DEPS := $(SRC:.c=.d)

CPPFLAGS = -MMD -MP
CPPFLAGS += -I.
# _DEFAULT_SOURCE needed for getentropy
CPPFLAGS += -D_DEFAULT_SOURCE

CFLAGS = -std=c23
CFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CFLAGS += -O3 -flto=auto -maes
CFLAGS += -fPIC -ffreestanding -g

# default = rv
ARFLAGS = rscv

#LDFLAGS +=

#LDLIBS +=

all: $(ANAME) $(SONAME_2) $(BINS)

$(OBJ): $(SRC)
	$(CC) -c $(CPPFLAGS) $(CFLAGS) $< -o $@

$(ANAME): $(OBJ)
	$(AR) $(ARFLAGS) $@ $<

$(SONAME_2): $(OBJ)
	$(CC) -o $@ -shared -nostdlib -Wl,-soname,$(SONAME_1) $(LDFLAGS) $<
	@ln -s -f --verbose -- $@ $(SONAME_1)
	@ln -s -f --verbose -- $@ $(SONAME_0)

clean:
	@$(RM) --verbose -- $(DEPS) $(OBJ) $(ANAME) $(SONAME_0) $(SONAME_1) $(SONAME_2)

lint:
	-clang-tidy --quiet $(SRC) -- $(CPPFLAGS) $(CFLAGS)

# https://www.gnu.org/software/make/manual/make.html#Phony-Targets
.PHONY: all clean lint

-include $(DEPS)
