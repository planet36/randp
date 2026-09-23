# SPDX-FileCopyrightText: Steven Ward
# SPDX-License-Identifier: MPL-2.0

export LC_ALL = C

VERSION := $(shell git describe --tags --abbrev=0 2> /dev/null)
ifeq ($(VERSION),)
    $(error Cannot determine VERSION: not a git checkout with tags, or "git describe" failed. Pass VERSION=X.Y explicitly, e.g., `make VERSION=1.0 install`)
endif
VERSION_SPLIT := $(subst ., , $(VERSION))
VERSION_MAJOR := $(word 1, $(VERSION_SPLIT))
VERSION_MINOR := $(word 2, $(VERSION_SPLIT))

LIBNAME := randp
ANAME := lib$(LIBNAME).a
# "versionless" soname
SONAME_0 := lib$(LIBNAME).so
# soname
SONAME_1 := $(SONAME_0).$(VERSION_MAJOR)
# "realname"
SONAME_2 := $(SONAME_1).$(VERSION_MINOR)
SINGLE_HEADER := $(LIBNAME)-single.h
PCFILE := $(LIBNAME).pc

# paths
PREFIX ?= /usr/local
BINDIR ?= $(PREFIX)/bin
INCDIR ?= $(PREFIX)/include
LIBDIR ?= $(PREFIX)/lib
MANDIR ?= $(PREFIX)/share/man
PKGCONFIGDIR ?= $(LIBDIR)/pkgconfig

DEPS = $(addsuffix .d,$(basename $(SRCS)))
OBJS = $(addsuffix .o,$(basename $(SRCS)))
BINS = $(basename $(SRCS))

# clang and clang++ not supported
CC = gcc
CXX = g++

CPPFLAGS = -MMD -MP
# _DEFAULT_SOURCE needed for getentropy, arc4random
CPPFLAGS += -D_DEFAULT_SOURCE

COMMON_COMPILE_FLAGS = -pipe -Wall -Wextra -Wpedantic -Wfatal-errors -Wundef
COMMON_COMPILE_FLAGS += -O3 -flto=auto
#COMMON_COMPILE_FLAGS += -march=native
#COMMON_COMPILE_FLAGS += -march=x86-64-v2 -maes
COMMON_COMPILE_FLAGS += -march=x86-64-v3 -maes -mvaes # x86-64-v3 implies avx, avx2
#COMMON_COMPILE_FLAGS += -march=raptorlake
COMMON_COMPILE_FLAGS += -Wno-unused-function
COMMON_COMPILE_FLAGS += -Wno-ignored-attributes

CFLAGS = -std=c23
CFLAGS += $(COMMON_COMPILE_FLAGS)

CXXFLAGS = -std=c++23
CXXFLAGS += $(COMMON_COMPILE_FLAGS)

#LDFLAGS +=

#LDLIBS +=

# default = rv
ARFLAGS = rscv
