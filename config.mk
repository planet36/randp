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

DEPS = $(addsuffix .d,$(basename $(SRCS)))
OBJS = $(addsuffix .o,$(basename $(SRCS)))
BINS = $(basename $(SRCS))

CPPFLAGS = -MMD -MP
# _DEFAULT_SOURCE needed for getentropy, arc4random
CPPFLAGS += -D_DEFAULT_SOURCE

CFLAGS = -std=c23
CFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CFLAGS += -O2 -flto=auto -maes

CXXFLAGS = -std=c++23
CXXFLAGS += -pipe -Wall -Wextra -Wpedantic -Wfatal-errors
CXXFLAGS += -O2 -flto=auto -maes

#LDFLAGS +=

#LDLIBS +=

# default = rv
ARFLAGS = rscv
